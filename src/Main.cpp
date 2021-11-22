#include <Glad/gl.h>
#include <GLFW/glfw3.h>

#include <KRE/KRE.hpp>
#include <GLM/glm.hpp>

#include <iostream>
#include <string>

#include "Window.hpp"

int main(int argc, char* argv[])
{
    SceneType scene = Scene_Lighting;
    if (argc > 1)
    {
        int value = std::stoi(argv[1]);
        if (value < minScene || value > maxScene)
        {
            std::cerr << "Invalid Range\n";
            return -1;
        }

        scene = (SceneType)value;
    }

    const float screenWidth = 1280;
    const float screenHeight = 720;

    Window::setScreenSize({ screenWidth, screenHeight });

    Window::changeScene(scene, true);
    Window::init();
    Window::uploadDataToCompute();
    Window::run();

    glfwTerminate();

    return 0;
}