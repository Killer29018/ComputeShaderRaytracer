#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "Window.hpp"
#include "Scene.hpp"
#include "ImguiWindowManager.hpp"

#include "KRE/KRE.hpp"

class Application
{
private:
    static Window m_Window;
    static Scene m_Scene;
    static glm::vec2 m_WindowSize;

    static KRE::Camera m_Camera;
public:
    static void init(glm::vec2 windowSize);
    static void run();
private:
    Application() = default;
};

#endif