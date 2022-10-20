#include "../headers/screen.h"
// Re-commented here as well as in 'screen.h' as 
// intellisence does not equate 'Class:attribute' to 'attribute'.

// The window width in non-pixel units.
int Screen::windowWidth         = 800;
// The window height in non-pixel units.
int Screen::windowHeight        = 600;
// The viewport width in pixels.
int Screen::frameBufferWidth    = 0;
// The viewport height in pixels.
int Screen::frameBufferHeight   = 0;
// The aspect ratio of the window.
float Screen::windowAspect = (float)Screen::windowWidth/Screen::windowHeight;

// Screen holds the values related to the window dimensions.
// It is mostly acessed to find the center of the screen throughout the program.
Screen::Screen(){   
};

// Set the values to match the dimensions of the screen given by GLFW.
void Screen::updateAndResize(GLFWwindow *window, int height = 0, int width = 0){
    Screen::recordDimensions(window);
    Screen::resizeViewport();
};

void Screen::resizeViewport(){
    // Adjust the width to match the aspect ratio.
    int width = Screen::frameBufferHeight * Screen::windowAspect;
    int left = (Screen::frameBufferWidth - width) / 2;
    int height = Screen::frameBufferHeight;
    glViewport(left, 0, width, height);
}

void Screen::recordDimensions(GLFWwindow *window){
    // Record the new window size.
    glfwGetWindowSize(window, &Screen::windowWidth, &Screen::windowHeight);
    // Record the new framebuffer size.
    glfwGetFramebufferSize(window, &Screen::frameBufferWidth, &Screen::frameBufferHeight);
}
