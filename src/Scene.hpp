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
    KRE::Camera* m_Camera;
    glm::vec2 m_WindowSize;

    std::vector<Shape> m_Scene;

    ConstantData m_Data;

    KRE::ComputeShader m_ComputeShader;
    KRE::Shader m_GeneralShader;
    KRE::VertexArray m_VAO;

    unsigned int m_SceneSSBO;
    unsigned int m_DataSSBO;

    unsigned int m_OutputImage;
    unsigned int m_DataImage;

    float m_SampleCount = 0.0f;
    float m_MaxSamples = 1000.0f;

    std::vector<glm::ivec2> m_ImageSizes;
    unsigned int m_CurrentImageSize = 0;

    bool m_Updated = false;
public:
    Scene() = default;
    ~Scene() = default;

    void init(KRE::Camera* camera, glm::vec2& windowSize);

    void setScreenSize(glm::vec2 windowSize);

    void renderImgui();
    void changeScene(SceneType scene);

    void addShape(const Shape& shape);
    std::vector<Shape>& getScene();
private:
    void createTexture(unsigned int& image, int width, int height, int bindPort, bool createTexture = true);
    void uploadDataToCompute();

    void setupVAO();
    void setupShaders();

    void resetData();

    void renderCompute();
    void renderScene();
    void renderImguiData();

    void updateTextureSizes();

    void randomScene();
    void simpleLight();
    void cornellSmoke();
    void cornellBox();
};

#endif