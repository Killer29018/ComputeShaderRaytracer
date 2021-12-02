#include "Application.hpp"

Window Application::m_Window;
Scene Application::m_Scene;
glm::vec2 Application::m_WindowSize;
KRE::Camera Application::m_Camera({ 0.0f, 0.0f });

void Application::init(glm::vec2 windowSize)
{
    m_WindowSize = windowSize;

    m_Window.setScreenSize(windowSize);
    m_Window.init();

    m_Camera = KRE::Camera(m_WindowSize,
        KRE::CameraPerspective::PERSPECTIVE,
        KRE::CameraMovementTypes::LOCKED_PERSPECTIVE);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cerr << "Failed to initialize OpenGL Context\n";
        exit(-1);
    }

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << "\n";

    glViewport(0, 0, m_WindowSize.x, m_WindowSize.y);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_Scene.init(&m_Camera, m_WindowSize);

    srand(time(0));

    ImguiWindowManager::init(m_Window.window);

    ImguiWindowManager::addWindow(m_Scene);
}

void Application::run()
{
    while(!glfwWindowShouldClose(m_Window.window))
    {
        KRE::Clock::tick();

        ImguiWindowManager::preRender();

        ImguiWindowManager::render();

        ImguiWindowManager::postRender();

        glfwSwapBuffers(m_Window.window);
        glfwPollEvents();
    }
}

void Application::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_Window.window, true);
    }

    switch (action)
    {
    case GLFW_PRESS: KRE::Keyboard::pressKey(key); break;
    case GLFW_RELEASE: KRE::Keyboard::unpressKey(key); break;
    }
}

void Application::GLFWResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    m_WindowSize = glm::vec2(width, height);
    m_Window.setScreenSize(m_WindowSize);
    m_Scene.setScreenSize(m_WindowSize);
}