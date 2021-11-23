#include <Glad/gl.h>
#include <GLFW/glfw3.h>

#include <KRE/KRE.hpp>
#include <GLM/glm.hpp>

#include <iostream>
#include <string>

#include "Window.hpp"
#include "Scene.hpp"

Window window;
Scene scene;

void run();

int main(int argc, char* argv[])
{
    SceneType sceneType = Scene_CornellSmoke;
    if (argc > 1)
    {
        int value = std::stoi(argv[1]);
        if (value < minScene || value > maxScene)
        {
            std::cerr << "Invalid Range\n";
            return -1;
        }

        sceneType = (SceneType)value;
    }

    const float screenWidth = 1280;
    const float screenHeight = 720;
    glm::vec2 windowSize = glm::vec2(screenWidth, screenHeight);

    KRE::Camera camera(windowSize, KRE::CameraPerspective::PERSPECTIVE, KRE::CameraMovementTypes::LOCKED_PERSPECTIVE);

    window.setScreenSize(windowSize);
    window.init();
    scene.init(&camera, &windowSize);
    scene.changeScene(sceneType);

    run();

    glfwTerminate();

    return 0;
}

void run()
{
    while (!glfwWindowShouldClose(window.window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool showDemoWindow = true;
        ImGui::ShowDemoWindow(&showDemoWindow);

        KRE::Clock::tick();

        scene.render();

        ImGui::Render();
        int displayW, displayH;
        glfwGetFramebufferSize(window.window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }
}