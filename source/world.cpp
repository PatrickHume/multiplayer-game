#include"../headers/world.h"

World::World(const unsigned int width, const unsigned int height) : 
defaultShader("resources/shaders/default.vert","resources/shaders/default.frag"),
outlineShader("resources/shaders/outline.vert","resources/shaders/outline.frag"),
textShader("resources/shaders/text.vert","resources/shaders/text.frag"),
ladaModel("resources/models/lada/scene.gltf"),
cubeModel("resources/models/cube/scene.gltf"),
camera(width, height, glm::vec3(0.0f, 0.0f, 10.0f))
{
    World::width = width;
    World::height = height;

    physicsWorldSettings.gravity = rp3d::Vector3(0, 0, 0); 
    // Create a physics world 
    physicsWorld = physicsCommon.createPhysicsWorld(physicsWorldSettings); 

    objects.push_back(Object(physicsWorld, &ladaModel, rp3d::BodyType::DYNAMIC));
    objects.push_back(Object(physicsWorld, &cubeModel, rp3d::BodyType::STATIC));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    textShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // We can reuse these models many times per frame

    ladaModel.setModelScale(glm::vec3(0.05f,0.05f,0.05f));
    ladaModel.updateLocal();

    // Force vector (in Newton) 
    rp3d::Vector3 force(2.0, 0.0, 0.0); 
    // Point where the force is applied 
    rp3d::Vector3 point(4.0, 5.0, 6.0); 

    // Apply a force to the body 
    //lada.body->applyLocalForceAtLocalPosition(force, point);

    //user.selectObject(&lada);

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	defaultShader.Activate();
	glUniform4f(glGetUniformLocation(defaultShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(defaultShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    //timer for physics
    double lastPhysicsTime = glfwGetTime();
    //timer for framerate
    double lastTime = glfwGetTime();
    
}

void World::ProcessInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (firstPress(window, GLFW_KEY_GRAVE_ACCENT) && camera.focus)
    {
        glfwSetCursorPos(window, (width / 2), (height / 2));
        interface.toggleCmdLine();
        camera.locked = interface.cmdLineOpen;
    }

    if (firstPress(window, GLFW_KEY_ENTER) && interface.checkCmd())
    {
        ObjectID objectId;
        float x;
        float y;
        float z;

        Command command = interface.getCmd(); // set interface.commandSent = false;
        switch (command.id){
            case CommandID::SUMMON_OBJECT:
                std::cout << "SUMMON_OBJECT" << std::endl;
                objectId = interface.mapObject(command.parameters[0]);
                std::cout << (int)objectId << std::endl;
                break;

            case CommandID::SUMMON_OBJECT_AT_POS:
                std::cout << "SUMMON_OBJECT_AT_POS" << std::endl;
                objectId = interface.mapObject(command.parameters[0]);
                x = std::stof(command.parameters[1]);
                y = std::stof(command.parameters[2]);
                z = std::stof(command.parameters[3]);
                std::cout << (int)objectId << " " << x << " "  << y << " "  << z << std::endl;
                break;

            case CommandID::LIST_OBJECTS:
                std::cout << "LIST_OBJECTS" << std::endl;
                break;

            case CommandID::SHOW_COLLIDERS:
                std::cout << "SHOW_COLLIDERS" << std::endl;
                break;

            case CommandID::HIDE_COLLIDERS:
                std::cout << "HIDE_COLLIDERS" << std::endl;
                break;

            case CommandID::ERROR:
                std::cout << "ERROR" << std::endl;
                break;

            default:
                throw std::runtime_error("Main ProcessInput: CommandID not found");
                break;
        }

    }

    if(interface.cmdLineOpen && camera.focus){
        for(int key = 44; key <= 93; key++){
            if (firstPress(window, key)){
                interface.typeChar(glfwGetKeyName(key, 0)[0]);
            }
        }
        if (firstPress(window, GLFW_KEY_SPACE)){
            interface.typeChar(' ');
        }
        if (firstPress(window, GLFW_KEY_BACKSPACE)){
            interface.backspace();
        }
    }

    camera.Inputs(window);
}

void World::Update(){
    // physics
    // -------
    // get the elapsed frame time
    currentPhysicsTime = glfwGetTime();
    elapsedPhysicsTime = (rp3d::decimal)(currentPhysicsTime - lastPhysicsTime);
    lastPhysicsTime = glfwGetTime();
    // update the physics world
    accumulatedPhysicsTime += elapsedPhysicsTime;
    while(accumulatedPhysicsTime > timeStep){
        accumulatedPhysicsTime -= timeStep;
        physicsWorld->update(timeStep);
    } 

    // framerate
    // ---------
    currentTime = glfwGetTime();
    frames++;
    if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
        // printf and reset timer
        std::cout << frames << " fps" << std::endl;
        frames = 0;
        lastTime += 1.0;
    }
}

void World::Draw(){
        // render
        // ------
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
        
        camera.updateMatrix(45.0f, 0.1f, 10000.0f);

        for(int i = 0; i < objects.size(); i++){
            objects[i].Draw(defaultShader, camera);
        }

        user.drawSelected(outlineShader, camera);

        interface.Draw(textRenderer, textShader, currentTime);
}

void World::Delete(){
    // Destroy the physics world 
    physicsCommon.destroyPhysicsWorld(physicsWorld);
	// Delete all the objects we've created
	defaultShader.Delete();
}

bool World::firstPress(GLFWwindow *window, int key){
    if (key >= 260){
        std::cout << (char)key << std::endl;
        throw std::runtime_error("Key checked not in range. (key code is > 96)");
    }

    if (glfwGetKey(window, key) == GLFW_PRESS){
        if(!keyIsHeld[key]){
            keyIsHeld[key] = true;
            return true;
        }
    }else{
        keyIsHeld[key] = false;
    }
    return false;
}
