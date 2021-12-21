#include "Application.hpp"

Window Application::m_Window;
Scene Application::m_Scene;
ContentBrowser Application::m_ContentBrowser;
glm::vec2 Application::m_WindowSize;

void Application::init(glm::vec2 windowSize)
{
    m_WindowSize = windowSize;

    m_Window.setScreenSize(windowSize);
    m_Window.init();

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

    m_Scene.init(m_WindowSize);

    srand(time(0));

	m_ContentBrowser.init(&m_Scene);

    ImguiWindowManager::init(m_Window.window);

    ImguiWindowManager::addWindow(m_Scene);
    ImguiWindowManager::addWindow(m_ContentBrowser);
}

void Application::run()
{
    while(!glfwWindowShouldClose(m_Window.window))
    {
        KRE::Clock::tick();
        processKeys();

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

void Application::processKeys()
{
    float dt = KRE::Clock::deltaTime;
    if (KRE::Keyboard::getKey(GLFW_KEY_W)) m_Scene.moveCamera(KRE::CameraMovement::FORWARD, dt);
    if (KRE::Keyboard::getKey(GLFW_KEY_S)) m_Scene.moveCamera(KRE::CameraMovement::BACK, dt);
    if (KRE::Keyboard::getKey(GLFW_KEY_A)) m_Scene.moveCamera(KRE::CameraMovement::LEFT, dt);
    if (KRE::Keyboard::getKey(GLFW_KEY_D)) m_Scene.moveCamera(KRE::CameraMovement::RIGHT, dt);
    if (KRE::Keyboard::getKey(GLFW_KEY_SPACE)) m_Scene.moveCamera(KRE::CameraMovement::UP, dt);
    if (KRE::Keyboard::getKey(GLFW_KEY_LEFT_CONTROL)) m_Scene.moveCamera(KRE::CameraMovement::DOWN, dt);


    m_Scene.getCamera().fastMovement = KRE::Keyboard::getKey(GLFW_KEY_LEFT_SHIFT);

}
