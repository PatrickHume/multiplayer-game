#include"../headers/world.h"

// World loads and stores many of the game assets.
// It is also where most of the game logic happens.
World::World(GLFWwindow *window)
{
    defaultShader.Load(   "resources/shaders/default.vert",   "resources/shaders/default.frag");
    blankShader.Load(     "resources/shaders/blank.vert",     "resources/shaders/blank.frag");
    instancedShader.Load( "resources/shaders/instanced.vert", "resources/shaders/default.frag");
    outlineShader.Load(   "resources/shaders/outline.vert",   "resources/shaders/outline.frag");
    textShader.Load(      "resources/shaders/text.vert",      "resources/shaders/text.frag");
    idShader.Load(        "resources/shaders/id.vert",        "resources/shaders/id.frag");
    
    ladaModel.Load(   "resources/models/lada/scene.gltf");
    cubeModel.Load(   "resources/models/cube/scene.gltf");
    floorModel.Load(  "resources/models/plane/scene.gltf");

    // resize lada to accurate size
    ladaModel.setModelScale(glm::vec3(0.019f,0.019f,0.019f));
    //ladaModel->setMass(930.0f);

    //resize the cube a length of 1 unit (the model is length 2)
    cubeModel.setModelScale(glm::vec3(0.5f,0.5f,0.5f));

    //resize the ground to 40, 1, 40
    floorModel.setModelScale(glm::vec3(20.0f,1.0f,20.0f));

    camera.setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    
    // generate render and frame buffer objects
    glGenRenderbuffers( 1, &renderbufId1 );
    glGenRenderbuffers( 1, &depthbufId   );
    glGenFramebuffers ( 1, &framebufId   );

    // setup second renderbuffer (triID)
    glBindRenderbuffer(GL_RENDERBUFFER, renderbufId1);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB32I, Screen::frameBufferWidth, Screen::frameBufferHeight);

    // setup depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, depthbufId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, Screen::frameBufferWidth, Screen::frameBufferHeight);

    // setup framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebufId);  
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, renderbufId1);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_RENDERBUFFER, depthbufId  );
    glEnable(GL_DEPTH_TEST);

    // check if everything went well
    GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);  
    if(stat != GL_FRAMEBUFFER_COMPLETE) { exit(0); }

    // setup color attachments
    const GLenum att[] = {GL_COLOR_ATTACHMENT1};
    glDrawBuffers(1, att);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //add model names from stringToModel to array
    for(std::map<std::string, Model*>::iterator it = mapModels.begin(); it != mapModels.end(); ++it) {
        modelNames.push_back(it->first);
    }

    physicsWorldSettings.gravity = rp3d::Vector3(0.0, -9.8, 0.0); 
    // Create a physics world 
    physicsWorld = physicsCommon.createPhysicsWorld(physicsWorldSettings); 

    //objects.push_back(Object(physicsWorld, &ladaModel, rp3d::BodyType::DYNAMIC));
    //objects.push_back(Object(physicsWorld, &cubeModel, rp3d::BodyType::STATIC));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Screen::frameBufferWidth), 0.0f, static_cast<float>(Screen::frameBufferHeight));
    textShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // We can reuse these models many times per frame

    // Force vector (in Newton) 
    rp3d::Vector3 force(2.0, 0.0, 0.0); 
    // Point where the force is applied 
    rp3d::Vector3 point(4.0, 5.0, 6.0); 



    rp3d::Vector3 position(0, -5, 0); 
    rp3d::Quaternion orientation = rp3d::Quaternion::identity(); 
    rp3d::Transform transform(position, orientation); 

    Object ground(physicsWorld, &floorModel, rp3d::BodyType::STATIC);
    ground.body->setTransform(transform);

    rp3d::Vector3       boxHalfExtents = rp3d::Vector3(20.0f,1.0f,20.0f);
    rp3d::Vector3       boxPosition    = rp3d::Vector3(0.0f,-1.0f,0.0f);
    rp3d::Quaternion    boxOrientation = rp3d::Quaternion::identity();
    rp3d::Transform     boxTransform   = rp3d::Transform::identity();

    BoxCollider collider{
        boxHalfExtents,
        boxPosition,
        boxOrientation,
        boxTransform
    };

    rp3d::BoxShape* boxShape = physicsCommon.createBoxShape(collider.halfExtents);
    ground.addBoxCollider(collider, boxShape);

    objects.push_back(ground);


    // Apply a force to the body 
    //lada.body->applyLocalForceAtLocalPosition(force, point);

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	defaultShader.Activate();
	glUniform4f(glGetUniformLocation(defaultShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(defaultShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	instancedShader.Activate();
	glUniform4f(glGetUniformLocation(instancedShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(instancedShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    //timer for physics
    lastPhysicsTime = glfwGetTime();
    //timer for framerate
    lastTime = glfwGetTime();
    
}

void World::createObjectAtPos(Model* model, glm::vec3 pos, glm::vec3 vel){
    rp3d::Vector3 position(pos.x, pos.y, pos.z); 
    rp3d::Vector3 velocity(vel.x, vel.y, vel.z); 
    rp3d::Quaternion orientation = rp3d::Quaternion::identity(); 
    rp3d::Transform transform(position, orientation); 

    Object object(physicsWorld, model, rp3d::BodyType::DYNAMIC);
    object.body->setTransform(transform);
    object.body->setLinearVelocity(velocity);

    rp3d::Vector3       boxHalfExtents = rp3d::Vector3(0.5f,0.5f,0.5f);
    rp3d::Vector3       boxPosition    = rp3d::Vector3(0.0f,0.0f,0.0f);
    rp3d::Quaternion    boxOrientation = rp3d::Quaternion::identity();
    rp3d::Transform     boxTransform   = rp3d::Transform::identity();

    BoxCollider collider{
        boxHalfExtents,
        boxPosition,
        boxOrientation,
        boxTransform
    };

    rp3d::BoxShape* boxShape = physicsCommon.createBoxShape(collider.halfExtents);
    object.addBoxCollider(collider, boxShape);
    
    objects.push_back(object);
}

void World::createObject(Model* model){
    glm::vec3 pos = camera.getPositionInFront(20.0f);
    createObjectAtPos(model, pos);
}

void World::fireObject(Model* model, float speed){
    glm::vec3 pos = camera.getPositionInFront(20.0f);
    glm::vec3 vel = camera.getOrientation()*speed;
    createObjectAtPos(model, pos, vel);
}

void World::ProcessInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && camera.focus && !camera.locked)
        fireObject(&cubeModel, 20.0f);

    if (firstPress(window, GLFW_KEY_GRAVE_ACCENT) && camera.focus)
    {
        glfwSetCursorPos(window, (Screen::windowWidth / 2), (Screen::windowHeight / 2));
        interface.toggleCmdLine();
        camera.locked = interface.cmdLineOpen;
    }

    if (firstPress(window, GLFW_KEY_ENTER) && interface.checkCmd())
    {
        Command command = interface.getCmd(); // set interface.commandSent = false;
        if (mapCommands.find(command.name) != mapCommands.end()) {
            (this->*mapCommands[command.name])(command.parameters);
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
    rp3d::decimal elapsedPhysicsTime = (rp3d::decimal)(currentPhysicsTime - lastPhysicsTime);
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
        
        camera.updateMatrix(45.0f, 0.1f, 10000.0f);
        
        for(int i = 0; i < objects.size(); i++){
            Object& object       = objects[i];
            Model* model         = object.getModel();
            glm::mat4& transform = object.getGLMTransform();
            model->prepareInstance(transform);
        }
        for(int i = 0; i < models.size(); i++){
            models[i]->drawInstanced(defaultShader, instancedShader, camera);
        }

        if(user.hasSelectedObject()){
            user.getSelectedObject()->drawOutline(blankShader, outlineShader, camera);
        }

        /*    
        if(collidersAreVisible){
            for(int i = 0; i < objects.size(); i++){
                objects[i].drawColliders(defaultShader, camera, &cubeModel);
            }
        }
        */

        interface.Draw(textRenderer, textShader, currentTime);
}

void World::Delete(){
    // Destroy the physics world 
    physicsCommon.destroyPhysicsWorld(physicsWorld);
	// Delete all the objects we've created
	defaultShader.Delete();

    glDeleteFramebuffers(1, &framebufId);
    glDeleteRenderbuffers(1, &renderbufId1);
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

glm::vec3 World::stringVecToGlmVec3(std::vector<std::string>& params, int i){
    glm::vec3 vec(
        std::stof(params[i]),
        std::stof(params[i+1]),
        std::stof(params[i+2]));
    return vec;
}

Model* World::stringToModel(std::string& name){
    if (mapModels.find(name) == mapModels.end()) {
        throw std::runtime_error("Interface couldnt find \""+name+"\" in mapModels.");
    }
    return mapModels[name];
}
World::functionPointer World::stringToCommand(std::string& name){
    if (mapCommands.find(name) == mapCommands.end()) {
        throw std::runtime_error("Interface couldnt find \""+name+"\" in mapCommands.");
    }
    return mapCommands[name];
}

Object* World::getObjectById(int id){
    for(int i = 0; i < objects.size(); i++){
        if(objects[i].getId() == id){
            return &(objects[i]);
        }
    }
    throw std::runtime_error("World: getObjectById, object not found.");
}

void World::summonObject(std::vector<std::string> params){
    Model* model = stringToModel(params[0]);
    createObject(model);
}
void World::summonManyObjects(std::vector<std::string> params){
    int num = std::stoi(params[0]);
    for(int i = 0; i < num; i++){
        Model* model = stringToModel(params[1]);
        createObject(model);
    }
}
void World::summonObjectAtPos(std::vector<std::string> params){
    Model* model = stringToModel(params[0]);
    glm::vec3 pos = stringVecToGlmVec3(params, 1); // offset by 1 as position 0 in params is taken by the object name
    createObjectAtPos(model,pos);
}
void World::summonManyObjectsAtPos(std::vector<std::string> params){
    int num = std::stoi(params[0]);
    for(int i = 0; i < num; i++){
        Model* model = stringToModel(params[1]);
        glm::vec3 pos = stringVecToGlmVec3(params, 2); // offset by 1 as position 0 in params is taken by the object name
        createObjectAtPos(model,pos);
    }
}
void World::listObjects(std::vector<std::string> params){
    for(int i = 0; i < modelNames.size(); i++){
        Message message{
            MessageType::DATA,
            modelNames[i]
        };
        interface.write(message);
    }
}
void World::showColliders(std::vector<std::string> params){
    collidersAreVisible = true;
}
void World::hideColliders(std::vector<std::string> params){
    collidersAreVisible = false;
}
void World::saveColliders(std::vector<std::string> params){
    std::cout << "saveColliders" << std::endl;
}
void World::editColliders(std::vector<std::string> params){
    std::cout << "editColliders" << std::endl;
}
void World::selectObject(std::vector<std::string> params){
    glBindFramebuffer(GL_FRAMEBUFFER, framebufId);
    glBindRenderbuffer(GL_FRAMEBUFFER, renderbufId1);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(int i = 0; i < objects.size(); i++){
        Object& object = objects[i];
        Model* model = object.getModel();
        glm::mat4& transform = object.getGLMTransform();
        int id = object.getId();

        model->setTransform(transform);
        model->drawId(idShader, camera, id);
    }
    int data[0];
    GLint x = (int)(Screen::frameBufferWidth/2);
    GLint y = (int)(Screen::frameBufferHeight/2);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, data);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int id = data[0];

    Message message{
        MessageType::DATA,
        "Object ID: " + std::to_string(id) + " selected."
    };
    
    if(id){
        user.selectObject(getObjectById(id));
    }else{
        user.deselectObject();
    }

    interface.write(message);
}
void World::deselectObject(std::vector<std::string> params){
    user.deselectObject();
}
void World::addCollider(std::vector<std::string> params){
    std::cout << "addCollider" << std::endl;
}
void World::delCollider(std::vector<std::string> params){
    std::cout << "delCollider" << std::endl;
}
void World::nextCollider(std::vector<std::string> params){
    std::cout << "nextCollider" << std::endl;
}
void World::prevCollider(std::vector<std::string> params){
    std::cout << "prevCollider" << std::endl;
}
void World::getColliderPosition(std::vector<std::string> params){
    std::cout << "getColliderPosition" << std::endl;
}
void World::getColliderRotation(std::vector<std::string> params){
    std::cout << "getColliderRotation" << std::endl;
}
void World::getColliderScale(std::vector<std::string> params){
    std::cout << "getColliderScale" << std::endl;
}
void World::setColliderPosition(std::vector<std::string> params){
    std::cout << "setColliderPosition" << std::endl;
}
void World::setColliderRotation(std::vector<std::string> params){
    std::cout << "setColliderRotation" << std::endl;
}
void World::setColliderScale(std::vector<std::string> params){
    std::cout << "setColliderScale" << std::endl;
}
