#include "ImguiWindowManager.hpp"

ImGuiIO* ImguiWindowManager::io;

std::vector<ImguiWindow> ImguiWindowManager::m_Windows;
GLFWwindow* ImguiWindowManager::m_Window;

void ImguiWindowManager::init(GLFWwindow* window)
{
    m_Window = window;

    IMGUI_CHECKVERSION();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

void ImguiWindowManager::addWindow(ImguiWindow& window)
{
    m_Windows.emplace_back(window);
}

void ImguiWindowManager::preRender()
{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
}

void ImguiWindowManager::render()
{
}

void ImguiWindowManager::postRender()
{
    ImGui::Render();
    int displayW, displayH;
    glfwGetFramebufferSize(m_Window, &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}