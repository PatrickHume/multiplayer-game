#include "../headers/screen.h"

int Screen::windowWidth      = 800;
int Screen::windowHeight     = 600;
int Screen::frameBufferWidth     = 0;
int Screen::frameBufferHeight    = 0;
float Screen::windowAspect = (float)Screen::windowWidth/Screen::windowHeight;

Screen::Screen(){
    
};

void Screen::Resize(GLFWwindow *window){
    glfwGetWindowSize(window, &Screen::windowWidth, &Screen::windowHeight);
    glfwGetFramebufferSize(window, &Screen::frameBufferWidth, &Screen::frameBufferHeight);
    //Screen::frameBufferWidth = Screen::frameBufferHeight * Screen::windowAspect;
    //int w = height * Screen::windowAspect;           // w is width adjusted for aspect ratio
    //int left = (width - w) / 2;
};
