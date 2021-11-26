#include "Application.hpp"

Window Application::m_Window;
Scene Application::m_Scene;
glm::vec2 Application::m_WindowSize;
KRE::Camera Application::m_Camera({ 0.0f, 0.0f });

ImGuiIO* Application::m_io;

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

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << GLAD_VERSION_MINOR(version) << "\n";

    glViewport(0, 0, m_WindowSize.x, m_WindowSize.y);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_Scene.init(&m_Camera, m_WindowSize);
    m_Scene.changeScene(Scene_Lighting);

    srand(time(0));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO(); (void)m_io;
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_Window.window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}

void Application::run()
{
    while(!glfwWindowShouldClose(m_Window.window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool showDemoWindow = true;
        ImGui::ShowDemoWindow(&showDemoWindow);

        KRE::Clock::tick();

        m_Scene.render();

        ImGui::Render();
        int displayW, displayH;
        glfwGetFramebufferSize(m_Window.window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_Window.window);
        glfwPollEvents();
    }
}