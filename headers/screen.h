#ifndef SCREEN_CLASS_H
#define SCREEN_CLASS_H
#include <GLFW/glfw3.h>

class Screen{
    public:
        Screen();
        static void Resize(GLFWwindow *window);
        // viewport values
        static int x, y, width, height;
        
        static int frameBufferWidth, frameBufferHeight;

        static int windowWidth, windowHeight;
        static float windowAspect;
    private:
};

#endif