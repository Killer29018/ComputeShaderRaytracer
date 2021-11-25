#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "imgui.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <KRE/KRE.hpp>

class Window
{
public:
    GLFWwindow* window;
private:
    glm::vec2 m_WindowSize;

    ImGuiIO* io = nullptr;
public:
    Window() = default;

    void init();
    void processKeys();

    void run();

    void setScreenSize(glm::vec2 windowSize);

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
    void initGLFW();
};

#endif