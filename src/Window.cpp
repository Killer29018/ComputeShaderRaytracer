#include "Window.hpp"
#include <glm/gtc/random.hpp>

#include "Application.hpp"

void Window::init()
{
    initGLFW();

    glfwSwapInterval(0);

    glfwSetKeyCallback(window, Application::GLFWKeyCallback);
    glfwSetWindowSizeCallback(window, Application::GLFWResizeCallback);
}

void Window::processKeys()
{

}

void Window::setScreenSize(glm::vec2 windowSize) { m_WindowSize = windowSize; }

void Window::initGLFW()
{
    if (!glfwInit())
        exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y, "Raytracing Engine", NULL, NULL);

    if (!window)
    {
        std::cerr << "Failed to create Window\n";
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
}
