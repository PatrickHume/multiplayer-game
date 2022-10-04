
//https://learnopengl.com/code_viewer_gh.php?code=src%2F1.getting_started%2F2.1.hello_triangle%2Fhello_triangle.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"../headers/user.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{

    // This is a factory module that you can use to create physics 
    // world and other objects. It is also responsible for 
    // logging and memory management 
    rp3d::PhysicsCommon physicsCommon; 
 
    // Create a physics world 
    rp3d::PhysicsWorld* physicsWorld = physicsCommon.createPhysicsWorld(); 

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    // Generates Shader object using shaders defualt.vert and default.frag
    Shader defaultShader("resources/shaders/default.vert","resources/shaders/default.frag");
    // This shader is used to highlight selected objects with an outline
    Shader outlineShader("resources/shaders/outline.vert","resources/shaders/outline.frag");
    
    // We can reuse these models many times per frame

    Model ladaModel = Model("resources/models/lada/scene.gltf");
    ladaModel.setModelScale(glm::vec3(0.05f,0.05f,0.05f));
    //ladaModel.setModelOrientation(glm::vec3(1.0f,0.0f,1.0f));
    //ladaModel.setModelPosition(glm::vec3(20.0f,0.0f,0.0f));
    ladaModel.updateLocal();

    //Model f15Model = Model("resources/models/f-4/scene.gltf");
    //f15Model.setModelScale(glm::vec3(0.3f,0.3f,0.3f));
    //f15Model.updateLocal();

    Model cubeModel = Model("resources/models/cube/scene.gltf");

    Object lada(physicsWorld, &ladaModel, rp3d::BodyType::DYNAMIC);
    Object cube(physicsWorld, &cubeModel, rp3d::BodyType::STATIC);

    user.selectObject(&cube);

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

    Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 10.0f));

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
        processInput(window);

        //ladaModel.applyRotation(glm::vec3(0.5f,0.1f,0.0f),0.01f);

        // render
        // ------
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
        
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 10000.0f);

        lada.Draw(defaultShader, camera);
        cube.Draw(defaultShader, camera);

        user.drawSelected(outlineShader, camera);
        
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}