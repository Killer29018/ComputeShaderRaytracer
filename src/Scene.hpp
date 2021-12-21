#ifndef SCENE_HPP
#define SCENE_HPP

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <vector>

#include "Materials.hpp"
#include "Shapes.hpp"
#include "Data.hpp"

#include <KRE/KRE.hpp>

#include "ImguiWindow.hpp"

enum SceneType
{
    Scene_RandomSpheres = 0,
    Scene_Lighting = 1,
    Scene_CornellBox = 2,
    Scene_CornellSmoke = 3,
};

constexpr static const int minScene = 0;
constexpr static const int maxScene = 3;

class Scene : public ImguiWindow
{
public:
private:
    KRE::Camera m_Camera;
    glm::vec2 m_WindowSize;

    std::vector<Shape> m_Scene;

    ConstantData m_Data;

    KRE::ComputeShader m_ComputeShader;

	KRE::Texture m_PlayButton, m_StopButton;

    unsigned int m_SceneSSBO;
    unsigned int m_DataSSBO;

    unsigned int m_OutputImage;
    unsigned int m_DataImage;

    float m_SampleCount = 0.0f;
    float m_MaxSamples = 1000.0f;
    int m_MaxDepth = 10;

    std::vector<glm::ivec2> m_ImageSizes;
    unsigned int m_CurrentImageSize = 0;

    bool m_Updated = false;
    bool m_Vsync = false;
    bool m_EnableRaycasting = true;

	bool m_CurrentlyPlaying = false;

    float m_AverageFPS = 0.0f;
    int m_FPSCount = 0;

    glm::vec3 m_CameraDirection = glm::vec3(0.0f);
public:
    Scene() : m_Camera({0.0f, 0.0f}){}
    ~Scene() = default;

    void init(glm::vec2& windowSize);

    void setScreenSize(glm::vec2 windowSize);

    void renderImgui();

    void addShape(const Shape& shape);

    void setSceneAndData(std::vector<Shape>& scene, ConstantData& data);

    void moveCamera(KRE::CameraMovement movement, float dt);

    KRE::Camera& getCamera() { return m_Camera; }
private:
    void createTexture(unsigned int& image, int width, int height, int bindPort);
    void uploadSSBOs();
    void uploadDataSSBO();
    void uploadSceneSSBO();

    void updateCameraData();

    void setupShaders();

    void resetData();

    void renderCompute();
    void renderScene();
    void renderImguiData();
    void renderMenuBar();

    void updateTextureSizes();

    void cleanScene();
};

#endif
