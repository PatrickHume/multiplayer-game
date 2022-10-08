
//https://learnopengl.com/code_viewer_gh.php?code=src%2F1.getting_started%2F2.1.hello_triangle%2Fhello_triangle.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"../headers/world.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow *window, Interface &interface, Camera &camera);

int main()
{
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
    GLFWwindow* window = glfwCreateWindow(Screen::windowWidth, Screen::windowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
    //glViewport(0, 0, width, height);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //https://stackoverflow.com/questions/9264346/problems-outputting-gl-primitiveid-to-custom-frame-buffer-object-fbo

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    // when both depth and stencil tests pass we use the reference value later
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    Screen::Resize(window);
    World world(window);

    // main loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        world.ProcessInput(window);
        world.Update();
        world.Draw();
        //textRenderer.renderText(textShader, "This is some sample text", 25.0f, 25.0f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
      
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
    }

    world.Delete();
    	// Delete window before ending the program
	glfwDestroyWindow(window);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    int w = height * Screen::windowAspect;           // w is width adjusted for aspect ratio
    int left = (width - w) / 2;
    glViewport(left, 0, w, height); 
    Screen::Resize(window);
}
