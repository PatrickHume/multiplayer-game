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
// It is mostly acessed to find the center of the screen.
Screen::Screen(){   
};

// Set the values to match the dimensions of the screen given by GLFW.
void Screen::recordNewWindowDimensions(GLFWwindow *window){
    glfwGetWindowSize(window, &Screen::windowWidth, &Screen::windowHeight);
    glfwGetFramebufferSize(window, &Screen::frameBufferWidth, &Screen::frameBufferHeight);
};
