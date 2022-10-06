
//https://learnopengl.com/code_viewer_gh.php?code=src%2F1.getting_started%2F2.1.hello_triangle%2Fhello_triangle.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"../headers/user.h"
#include"../headers/textRenderer.h"
#include"../headers/stringSwitch.h"
#include"../headers/interface.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Interface &interface, Camera &camera);

// settings
const unsigned int width = 800;
const unsigned int height = 600;

int main()
{

    // This is a factory module that you can use to create physics 
    // world and other objects. It is also responsible for 
    // logging and memory management 
    rp3d::PhysicsCommon physicsCommon; 

    rp3d::PhysicsWorld::WorldSettings settings; 
    settings.gravity = rp3d::Vector3(0, 0, 0); 

    // Create a physics world 
    rp3d::PhysicsWorld* physicsWorld = physicsCommon.createPhysicsWorld(settings); 

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

    User user;
    TextRenderer textRenderer;
    Interface interface(width, height);

    // Generates Shader object using shaders defualt.vert and default.frag
    Shader defaultShader("resources/shaders/default.vert","resources/shaders/default.frag");
    // This shader is used to highlight selected objects with an outline
    Shader outlineShader("resources/shaders/outline.vert","resources/shaders/outline.frag");
    Shader textShader("resources/shaders/text.vert","resources/shaders/text.frag");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    textShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // We can reuse these models many times per frame

    Model ladaModel = Model("resources/models/lada/scene.gltf");
    ladaModel.setModelScale(glm::vec3(0.05f,0.05f,0.05f));
    ladaModel.updateLocal();

    //ladaModel.setModelOrientation(glm::vec3(1.0f,0.0f,1.0f));
    //ladaModel.setModelPosition(glm::vec3(20.0f,0.0f,0.0f));

    //Model f15Model = Model("resources/models/f-4/scene.gltf");
    //f15Model.setModelScale(glm::vec3(0.3f,0.3f,0.3f));
    //f15Model.updateLocal();

    Model cubeModel = Model("resources/models/cube/scene.gltf");

    Object lada(physicsWorld, &ladaModel, rp3d::BodyType::DYNAMIC);
    Object cube(physicsWorld, &cubeModel, rp3d::BodyType::STATIC);

    // Force vector (in Newton) 
    rp3d::Vector3 force(2.0, 0.0, 0.0); 
    // Point where the force is applied 
    rp3d::Vector3 point(4.0, 5.0, 6.0); 
    // Apply a force to the body 
    lada.body->applyLocalForceAtLocalPosition(force, point);

    user.selectObject(&lada);

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	defaultShader.Activate();
	glUniform4f(glGetUniformLocation(defaultShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(defaultShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    // when both depth and stencil tests pass we use the reference value later
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 10.0f));

    // length of physics frame in seconds
    const rp3d::decimal timeStep = 1.0f / 60.0f;

    //timer for physics
    double lastPhysicsTime = glfwGetTime();
    double currentPhysicsTime;

    rp3d::decimal elapsedPhysicsTime = 0;
    rp3d::decimal accumulatedPhysicsTime = 0;
    //timer for framerate
    double lastTime = glfwGetTime();
    int frames = 0;

    // main loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
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
        double currentTime = glfwGetTime();
        frames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            std::cout << frames << " fps" << std::endl;
            frames = 0;
            lastTime += 1.0;
        }
        
        // input
        // -----
        processInput(window, interface, camera);
        
        //ladaModel.applyRotation(glm::vec3(0.5f,0.1f,0.0f),0.01f);

        // render
        // ------
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
        
        camera.updateMatrix(45.0f, 0.1f, 10000.0f);

        lada.Draw(defaultShader, camera);
        cube.Draw(defaultShader, camera);

        user.drawSelected(outlineShader, camera);

        interface.Draw(textRenderer, textShader, currentTime);

        //textRenderer.renderText(textShader, "This is some sample text", 25.0f, 25.0f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
      
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
    }

    // Destroy the physics world 
    physicsCommon.destroyPhysicsWorld(physicsWorld);
	// Delete all the objects we've created
	defaultShader.Delete();
    	// Delete window before ending the program
	glfwDestroyWindow(window);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

bool keyIsHeld[260];

bool firstPress(GLFWwindow *window, int key){
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Interface &interface, Camera &camera)
{
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}