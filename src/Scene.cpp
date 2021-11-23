#include "Scene.hpp"

#include <glm/gtc/random.hpp>

void Scene::init(KRE::Camera* camera, glm::vec2* windowSize)
{
    m_Camera = camera;
    m_WindowSize = windowSize;

    setupVAO();
    setupShaders();
    resetData();
}

void Scene::render()
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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_OutputImage);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_DataImage);

        glDispatchCompute(m_WindowSize->x / localWorkGroupSize, m_WindowSize->y / localWorkGroupSize, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        m_SampleCount += 1.0f;
        std::cout << "\r" << "Samples : " << (int)m_SampleCount;
    }


    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_OutputImage);

    m_VAO.bind();
    m_GeneralShader.bind();

    constexpr unsigned int INDICES_COUNT = 6;
    glDrawElements(GL_TRIANGLES, INDICES_COUNT, GL_UNSIGNED_INT, NULL);
}

void Scene::changeScene(SceneType scene)
{
    m_Updated = true;
    m_Scene.clear();

    switch(scene)
    {
    case Scene_RandomSpheres:
        randomScene();
        m_Data.cameraPos = glm::vec3(13, 2, 3);
        m_Data.cameraLookAt = glm::vec3(0, 0, 0);
        m_Data.cameraFov = 20.0f;
        m_Data.cameraAperture = 0.1f;
        break;
    case Scene_Lighting:
        simpleLight();
        m_Data.background = glm::vec3(0.0f);
        m_Data.cameraPos = glm::vec3(26, 3, 6);
        m_Data.cameraLookAt = glm::vec3(0, 2, 0);
        m_Data.cameraFov = 20.0f;
        break;
    case Scene_CornellSmoke:
        cornellSmoke();
        m_Data.background = glm::vec3(0.0f);
        m_Data.cameraPos = glm::vec3(278, 278, -800);
        m_Data.cameraLookAt = glm::vec3(278, 278, 0);
        m_Data.cameraFov = 40.0f;
        break;
    case Scene_CornellBox:
        cornellBox();
        m_Data.background = glm::vec3(0.0f);
        m_Data.cameraPos = glm::vec3(278, 278, -800);
        m_Data.cameraLookAt = glm::vec3(278, 278, 0);
        m_Data.cameraFov = 40.0f;
        break;
    default:
        m_Data.background = glm::vec3(0.0f);
        break;
    }
}

std::vector<Shape>& Scene::getScene()
{
    return m_Scene;
}


void Scene::addShape(const Shape& shape)
{
    m_Scene.push_back(shape);
}

void Scene::createTexture(unsigned int& image, int width, int height, int bindPort)
{
    glGenTextures(1, &image);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

void Scene::setupVAO()
{
    KRE::Vertices vertices({
        // Position     Tex Coords
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f
    });

    KRE::Indices indices({
        0, 1, 2,
        1, 2, 3
    });

    m_VAO.init();
    KRE::VertexBuffer VBO(true);
    KRE::ElementArray EBO(true);

    m_VAO.bind();
    VBO.bind();

    VBO.setData(vertices);

    EBO.bind();
    EBO.setData(indices);

    VBO.setVertexAttrib(0, 2, 4, 0);
    VBO.setVertexAttrib(1, 2, 4, 2);

    VBO.unbind();
    m_VAO.unbind();
}

void Scene::setupShaders()
{
    m_GeneralShader.compilePath("res/shaders/basicVertexShader.vs.glsl", "res/shaders/basicFragmentShader.fs.glsl");
    m_GeneralShader.bind();
    m_GeneralShader.setUniformInt("u_Texture", 0);

    m_ComputeShader.compilePath("res/shaders/RaytracingCompute.comp.glsl");

    createTexture(m_OutputImage, m_WindowSize->x, m_WindowSize->y, 0);
    createTexture(m_DataImage, m_WindowSize->x, m_WindowSize->y, 1);

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
    m_Data.maxDepth = 10;
    m_Data.aspectRatio = m_WindowSize->x / m_WindowSize->y;
}

void Scene::randomScene()
{
    Material groundMat = Lambertian(glm::vec3(0.5, 0.5, 0.5));
    addShape(Sphere(glm::vec3(0, -1000, 0), 1000, groundMat));

    int maxSize = 7;

    #define random() (rand() % 1000) / 1000.0f

    for (int a = -maxSize; a < maxSize; a++)
    {
        for (int b = -maxSize; b < maxSize; b++)
        {
            float chooseMat = random();
            glm::vec3 center = glm::vec3(a + 0.9*random(), 0.2, b + 0.9*random());

            if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9)
            {
                Material mat;
                if (chooseMat < 0.8)
                {
                    glm::vec3 albedo = glm::linearRand(glm::vec3(0.0), glm::vec3(1.0));
                    mat = Lambertian(albedo);
                    addShape(Sphere(center, 0.2, mat));
                }
                else if (chooseMat < 0.95)
                {
                    glm::vec3 albedo = glm::linearRand(glm::vec3(0.5), glm::vec3(1.0));
                    float fuzz = random() / 0.5f;
                    mat = Metal(albedo, fuzz);
                    addShape(Sphere(center, 0.2, mat));
                }
                else
                {
                    mat = Dielectric(1.5);
                    addShape(Sphere(center, 0.2, mat));
                }
            }
        }
    }
    #undef random

    Material m1 = Dielectric(1.5);
    addShape(Sphere(glm::vec3(0, 1, 0), 1.0, m1));

    Material m2 = Lambertian(glm::vec3(0.4, 0.2, 0.1));
    addShape(Sphere(glm::vec3(-4, 1, 0), 1.0, m2));

    Material m3 = Metal(glm::vec3(0.7, 0.6, 0.5), 0.0);
    addShape(Sphere(glm::vec3(4, 1, 0), 1.0, m3));
}

void Scene::simpleLight()
{
    // Material ground = Metal(glm::vec3(1.0, 1.0, 0.0), 0.9);
    Material ground = Lambertian(glm::vec3(1.0, 1.0, 0.0));
    Material center = Lambertian(glm::vec3(1.0, 0.0, 0.0));
    addShape(Sphere(glm::vec3(0, -1000, 0), 1000, ground));
    addShape(Sphere(glm::vec3(0, 2, 0), 2, center));

    Material light1 = DiffuseLight(glm::vec3(4));
    addShape(XYRect(3, 2, 1, 2, -2, light1));
}

void Scene::cornellBox()
{
    Material red = Lambertian(glm::vec3(0.65, 0.05, 0.05));
    Material white = Lambertian(glm::vec3(0.73));
    Material green = Lambertian(glm::vec3(0.12, 0.45, 0.15));
    Material light = DiffuseLight(glm::vec3(15));

    addShape(YZRect(0, 555, 0, 555, 555, green));
    addShape(YZRect(0, 555, 0, 555, 0, red));
    addShape(XZRect(213, 130, 227, 105, 554, light));
    addShape(XZRect(0, 555, 0, 555, 0, white));
    addShape(XZRect(0, 555, 0, 555, 555, white));
    addShape(XYRect(0, 555, 0, 555, 555, white));

    Shape box1 = Cube(glm::vec3(130, 0, 65), glm::vec3(165), white);
    box1.rotateY(-18.0f);
    addShape(box1);

    Shape box2 = Cube(glm::vec3(265, 0, 295), glm::vec3(165, 330, 165), white);
    box2.rotateY(15.0f);
    addShape(box2);
}

void Scene::cornellSmoke()
{
    Material red = Lambertian(glm::vec3(0.65, 0.05, 0.05));
    Material white = Lambertian(glm::vec3(0.73));
    Material green = Lambertian(glm::vec3(0.12, 0.45, 0.15));
    Material light = DiffuseLight(glm::vec3(15));

    addShape(YZRect(0, 555, 0, 555, 555, green));
    addShape(YZRect(0, 555, 0, 555, 0, red));
    addShape(XZRect(213, 130, 227, 105, 554, light));
    addShape(XZRect(0, 555, 0, 555, 0, white));
    addShape(XZRect(0, 555, 0, 555, 555, white));
    addShape(XYRect(0, 555, 0, 555, 555, white));

    Material b1 = ConstantMedium(glm::vec3(1.0f), 0.02);
    Shape box1 = Cube(glm::vec3(130, 0, 65), glm::vec3(165), b1);
    box1.rotateY(-18.0f);
    addShape(box1);

    Material b2 = ConstantMedium(glm::vec3(0.0f), 0.01);
    Shape box2 = Cube(glm::vec3(265, 0, 295), glm::vec3(165, 330, 165), b2);
    box2.rotateY(15.0f);
    addShape(box2);
}