#include "Window.hpp"

#include <glm/gtc/random.hpp>

void Window::init()
{
    initGLFW();

    glfwSwapInterval(1);

    glfwSetKeyCallback(window, Window::KeyCallback);
}

void Window::processKeys()
{

}

void Window::setScreenSize(glm::vec2 windowSize) { m_WindowSize = windowSize; }

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    switch (action)
    {
    case GLFW_PRESS: KRE::Keyboard::pressKey(key); break;
    case GLFW_RELEASE: KRE::Keyboard::unpressKey(key); break;
    }
}

void Window::initGLFW()
{
    if (!glfwInit())
        exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y, "Raytracing", NULL, NULL);

    if (!window)
    {
        std::cerr << "Failed to create Window\n";
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
}