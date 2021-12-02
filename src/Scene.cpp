#include "Scene.hpp"

#include <sstream>

#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>

#include "SceneLoader.hpp"

void Scene::init(KRE::Camera* camera, glm::vec2& windowSize)
{
    m_Camera = camera;
    m_WindowSize = windowSize;

    m_ImageSizes = {
        {320, 180},
        {640, 360},
        {960, 540},
        {1280, 720},
        {1920, 1080}
    };
    m_CurrentImageSize = 4;

    // setupVAO();
    setupShaders();
    resetData();

    SceneLoader::loadFile("res/scenes/CornellSmoke.json", this);
    m_Updated = true;
}

void Scene::setScreenSize(glm::vec2 windowSize)
{
    m_WindowSize = windowSize;

    // m_Data.aspectRatio = m_WindowSize.x / m_WindowSize.y;
    m_Data.aspectRatio = 16.0/9.0;
    m_Updated = true;
}

void Scene::renderImgui()
{
    renderMenuBar();

    renderCompute();
    renderScene();
    renderImguiData();
}

void Scene::addShape(const Shape& shape)
{
    m_Scene.push_back(shape);
}

void Scene::setSceneAndData(std::vector<Shape>& scenes, ConstantData& data)
{
    m_Scene = scenes;
    m_Data = data;
}

void Scene::createTexture(unsigned int& image, int width, int height, int bindPort, bool createTexture)
{
    glActiveTexture(GL_TEXTURE0);

    if (!createTexture)
    {
        glDeleteTextures(1, &image);
    }

    glGenTextures(1, &image);
    glBindTexture(GL_TEXTURE_2D, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, image);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(bindPort, image, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void Scene::uploadDataToCompute()
{
    std::vector<Shape>& sceneData = m_Scene;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SceneSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shape) * sceneData.size(), sceneData.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DataSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ConstantData), &m_Data, GL_DYNAMIC_DRAW);
}

void Scene::setupShaders()
{
    m_ComputeShader.compilePath("res/shaders/RaytracingCompute.comp.glsl");

    glm::ivec2 currentImage = m_ImageSizes[m_CurrentImageSize];
    createTexture(m_OutputImage, currentImage.x, currentImage.y, 0);
    createTexture(m_DataImage, currentImage.x, currentImage.y, 1);

    glGenBuffers(1, &m_SceneSSBO);
    glGenBuffers(1, &m_DataSSBO);
}

void Scene::resetData()
{
    m_Data.cameraPos = glm::vec3(0.0, 0.0, 1.0);
    m_Data.cameraLookAt = glm::vec3(0.0, 0.0, 0.0);
    m_Data.cameraUp = m_Camera->up;
    m_Data.background = glm::vec3(0.7, 0.8, 1.0);
    m_Data.cameraViewDist = 1.0f;
    m_Data.cameraFocusDist = 10.0;
    m_Data.cameraFov = 40.0f;
    m_Data.cameraAperture = 0.0;
    m_Data.aspectRatio = 16.0/9.0;
}

void Scene::renderCompute()
{
    if (m_Updated)
    {
        uploadDataToCompute();
        m_Updated = false;
    }

    if (!(m_SampleCount >= m_MaxSamples))
    {
        int localWorkGroupSize = 16;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_SceneSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_DataSSBO);
        m_ComputeShader.bind();
        m_ComputeShader.setUniformFloat("u_SampleCount", m_SampleCount);
        m_ComputeShader.setUniformInt("u_MaxDepth", m_MaxDepth);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_OutputImage);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_DataImage);

        glm::ivec2 imageSize = m_ImageSizes[m_CurrentImageSize];
        glDispatchCompute(imageSize.x / localWorkGroupSize, imageSize.y / localWorkGroupSize, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        m_SampleCount += 1.0f;
    }
}

void Scene::renderScene()
{
    if (ImGui::Begin("Scene"))
    {
        ImGui::BeginChild("SceneRender");

        ImVec2 wSize = ImGui::GetWindowSize();

        ImGui::Image((ImTextureID)m_OutputImage, wSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();

        ImGui::End();
    }
}

void Scene::renderImguiData()
{
    static int quantity = 0.0f;
    static float currentAvgFPS = 0.0f;

    quantity++;
    currentAvgFPS += (KRE::Clock::deltaTime - currentAvgFPS)/(float)quantity;

    if (ImGui::Begin("Data"))
    {
        ImGui::Text("FPS: %f", (1.0f / KRE::Clock::deltaTime));
        ImGui::Text("AVG FPS: %f", 1.0f / currentAvgFPS);

        ImGui::Text("Sample Count: %i", (int)m_SampleCount);

        ImGui::Text("Object Count: %li", m_Scene.size());

        ImGui::End();
    }

    if (ImGui::Begin("Settings"))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.90f);

        ImGui::Text("Max Samples:");
        ImGui::SliderFloat("###MaxSamples", &m_MaxSamples, 100, 20000, "%0.0f");
        ImGui::Text("Max Depth:");
        ImGui::SliderInt("###MaxDepth", &m_MaxDepth, 1, 500);

        ImGui::NewLine();

        ImGuiComboFlags flags = ImGuiComboFlags_NoArrowButton;
        glm::ivec2 v = m_ImageSizes[m_CurrentImageSize];
        std::stringstream s;
        s << v.x << " : " << v.y;
        if (ImGui::BeginCombo("###ImageSize", s.str().c_str(), flags))
        {
            for (int n = 0; n < m_ImageSizes.size(); n++)
            {
                const bool isSelected = (m_CurrentImageSize == n);

                glm::ivec2 value = m_ImageSizes[n];
                std::stringstream stringValue;
                stringValue << value.x << " : " << value.y;

                if (ImGui::Selectable(stringValue.str().c_str(), isSelected))
                {
                    m_CurrentImageSize = n;
                    m_SampleCount = 0;
                    updateTextureSizes();
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        static bool vsync = true;
        if (ImGui::Checkbox("Vsync", &vsync))
        {
            glfwSwapInterval(vsync);
            currentAvgFPS = 0.0f;
        }

        if (ImGui::Button("Clear Textures", ImVec2(ImGui::GetWindowWidth() * 0.90f, 0)))
        {
            updateTextureSizes();
            m_SampleCount = 0.0f;
        }

        ImGui::PopItemWidth();

        ImGui::End();
    }
}

void Scene::renderMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            ImGui::MenuItem("This is a Menu", NULL, false, false);
            // Add File Loading and Saving
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Scene::updateTextureSizes()
{
    glm::ivec2 currentImage = m_ImageSizes[m_CurrentImageSize];
    createTexture(m_OutputImage, currentImage.x, currentImage.y, 0, false);
    createTexture(m_DataImage, currentImage.x, currentImage.y, 1, false);
}