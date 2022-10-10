#ifndef SCREEN_CLASS_H
#define SCREEN_CLASS_H
#include <GLFW/glfw3.h>

// Screen holds the values related to the window dimensions.
// It is mostly acessed to find the center of the screen throughout the program.
class Screen{
    public:
        Screen();
        static void recordNewWindowDimensions(GLFWwindow *window);
        // The viewport width in pixels.
        static int frameBufferWidth;
        // The viewport height in pixels.
        static int frameBufferHeight;
        // The window width in non-pixel units.
        static int windowWidth;
        // The window height in non-pixel units.
        static int windowHeight;
        // The aspect ratio of the window.
        static float windowAspect;
    private:
};

#endif