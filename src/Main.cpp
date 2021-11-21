#include <Glad/gl.h>
#include <GLFW/glfw3.h>

#include <KRE/KRE.hpp>
#include <GLM/glm.hpp>

#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "STB\stb_image.h"

#include "Window.hpp"

int main(int argc, char* argv[])
{
    SceneType scene = Scene_FinishedCornellBox;
    if (argc > 1)
    {
        int value = std::stoi(argv[1]);
        if (value < 0 || value > 2)
        {
            std::cerr << "Invalid Range\n";
            return -1;
        }

        scene = (SceneType)value;
    }

    float aspectRatio = 16.0 / 9.0;
    float screenWidth = 1280;

    switch (scene)
    {
    case Scene_FinishedCornellBox:
    case Scene_CornellBox:
        aspectRatio = 1.0f;
        screenWidth = 640;
    }

    float screenHeight = screenWidth / aspectRatio;

    Window::aspectRatio = aspectRatio;

    Window::init({ screenWidth, screenHeight });
    Window::changeScene(scene);
    Window::uploadDataToCompute();
    Window::run();

    glfwTerminate();

    return 0;
}