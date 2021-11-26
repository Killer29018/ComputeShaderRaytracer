#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <KRE/KRE.hpp>

class Window
{
public:
    GLFWwindow* window;
private:
    glm::vec2 m_WindowSize;
public:
    Window() = default;

    void init();
    void processKeys();

    void setScreenSize(glm::vec2 windowSize);

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
    void initGLFW();
};

#endif