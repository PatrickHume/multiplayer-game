
//https://learnopengl.com/code_viewer_gh.php?code=src%2F1.getting_started%2F2.1.hello_triangle%2Fhello_triangle.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../headers/world.h"

// Include gperftools performance profiler if PROFILE is defined.
// ./showprof.sh will open up the result of the profiler.
#ifdef PROFILE
#include <gperftools/profiler.h>
#endif // PROFILE

// Called when the window is resized, resizes the viewport to match.
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main()
{
    // Initialize and configure GLFW.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create a GLFW window.
    GLFWwindow* window = glfwCreateWindow(Screen::windowWidth, Screen::windowHeight, "multiplayer-game", NULL, NULL);
    if (window == NULL)
    {// Window creation error handling.
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window.");
    }
    // Set OpenGl current contex to this window.
    glfwMakeContextCurrent(window);
    // Resize the viewport on window resize.
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Load OpenGL function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    
    // Bind the frame buffer to 0, the frame that is drawn to the viewport.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Enable depth and stencil testing.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // Only overwrite the pixel if both depth and stencil tests pass.
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // Record the window dimensions in the screen class.
    Screen::recordNewWindowDimensions(window);

    // Create the world - this is where most of the game logic happens.
    World world(window);

    // Name the output file "output.pprof" and begin profiling if debugging.
    #ifdef PROFILE
    ProfilerStart("output.pprof");
    #endif // PROFILE

    // Run the mainloop until the window is closed by the user.
    while (!glfwWindowShouldClose(window))
    {
        // Pass the user input to world.
        world.ProcessInput(window);
        // Update the physics of all objects in world.
        world.Update();
        // Render the world to the back buffer.
        world.Draw();

		// Swap the back buffer with the front buffer and display to the viewport.
		glfwSwapBuffers(window);
		// Poll all GLFW events.
		glfwPollEvents();
    }

    // Output the profiling file if debugging.
    #ifdef PROFILE
    ProfilerStop();
    #endif // PROFILE

    // Delete the world.
    world.Delete();
    // Delete the window before ending the program.
	glfwDestroyWindow(window);
    // Clear all previously allocated GLFW resources.
    glfwTerminate();

    return 0;
}

// Resize the viewport on window resize.
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Adjust the width to match the aspect ratio.
    int w = height * Screen::windowAspect;
    int left = (width - w) / 2;
    // Set the viewport size.
    glViewport(left, 0, w, height); 
    // Record the new window dimensions in Screen.
    Screen::recordNewWindowDimensions(window);
}
