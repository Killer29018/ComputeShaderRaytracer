#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/gl.h>
#include <glfw/glfw3.h>

#include <KRE/KRE.hpp>

#include "Scene.hpp"
#include "Data.hpp"

enum SceneType
{
    Scene_RandomSpheres = 0,
    Scene_Lighting = 1,
    Scene_CornellBox = 2,
    Scene_CornellSmoke = 3,
};

class Window
{
public:
    static GLFWwindow* window;
    static KRE::Camera camera;
    static ConstantData data;

    static Scene scene;

    static float aspectRatio;

    static float maxSamples;
private:
    static KRE::ComputeShader m_ComputeShader;
    static KRE::Shader m_GeneralShader;
    static KRE::VertexArray m_VAO;

    static glm::vec2 m_WindowSize;

    static unsigned int m_SceneSSBO;
    static unsigned int m_DataSSBO;

    static unsigned int m_OutputImage;
    static unsigned int m_DataImage;

    static float m_SampleCount;
public:
    static void init(glm::vec2 windowSize);
    static void processKeys();

    static void resetData();

    static void run();
    static void changeScene(SceneType scene);
    static void uploadDataToCompute();

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
    static void initGLFW();

    static void createTexture(unsigned int& image, int width, int height, int bindPort);

    static Scene randomScene();
    static Scene simpleLight();
    static Scene cornellBox();
    static Scene cornellSmoke();
};

#endif