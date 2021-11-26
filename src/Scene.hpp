#ifndef SCENE_HPP
#define SCENE_HPP

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <vector>

#include "Materials.hpp"
#include "Shapes.hpp"
#include "Data.hpp"

#include <KRE/KRE.hpp>

enum SceneType
{
    Scene_RandomSpheres = 0,
    Scene_Lighting = 1,
    Scene_CornellBox = 2,
    Scene_CornellSmoke = 3,
};

constexpr static const int minScene = 0;
constexpr static const int maxScene = 3;

class Scene
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

    float m_SampleCount;
    float m_MaxSamples = 10000.0f;

    bool m_Updated = false;
public:
    Scene() = default;
    ~Scene() = default;

    void init(KRE::Camera* camera, glm::vec2& windowSize);

    void render();
    void changeScene(SceneType scene);

    void addShape(const Shape& shape);
    std::vector<Shape>& getScene();
private:
    void createTexture(unsigned int& image, int width, int height, int bindPort);
    void uploadDataToCompute();

    void setupVAO();
    void setupShaders();

    void resetData();

    void randomScene();
    void simpleLight();
    void cornellSmoke();
    void cornellBox();
};

#endif