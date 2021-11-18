#include <Glad/gl.h>
#include <GLFW/glfw3.h>

#include <KRE/KRE.hpp>
#include <GLM/glm.hpp>

#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "STB\stb_image.h"

#include "Window.hpp"

static const float ASPECT_RATIO = 1.0;
static const unsigned int SCREEN_WIDTH = 640;
static const unsigned int SCREEN_HEIGHT = SCREEN_WIDTH / ASPECT_RATIO;

int main()
{
    Window::init({ SCREEN_WIDTH, SCREEN_HEIGHT });

    Window::changeScene(3);
    Window::uploadDataToCompute();
    Window::run();

    glfwTerminate();

    return 0;
}