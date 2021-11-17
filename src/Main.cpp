#include <Glad/gl.h>
#include <GLFW/glfw3.h>

#include <KRE/KRE.hpp>
#include <GLM/glm.hpp>

#include <iostream>
#include <string>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/random.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "STB\stb_image.h"

#include "Scene.hpp"

static const float ASPECT_RATIO = 1.0;
static const unsigned int SCREEN_WIDTH = 640;
static const unsigned int SCREEN_HEIGHT = SCREEN_WIDTH / ASPECT_RATIO;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processKeys();

void createTexture(unsigned int& id, int width, int height, int bindPoint);

Scene randomScene();
Scene simpleLight();
Scene cornellBox();

struct ConstantData
{
    alignas(16) glm::vec3 cameraPos;
    alignas(16) glm::vec3 cameraLookAt;
    alignas(16) glm::vec3 cameraUp;
    float cameraViewDist;
    float cameraFocusDist;
    float cameraFov;
    float cameraAperture;
    unsigned int maxDepth;
    float aspectRatio;
};


int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raymarching", NULL, NULL);

    if (!window)
    {
        std::cerr << "Failed to create Window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyCallback);

    glfwSwapInterval(1);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cerr << "Failed to initialize Opengl Context\n";
        return -1;
    }

    std::cout << "Loaded Opengl " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << "\n";

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    srand(time(0));

    KRE::Vertices vertices({
        // Position
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f
    });

    KRE::Indices indices({
        0, 1, 2,
        1, 2, 3
    });

    KRE::VertexArray VAO(true);
    KRE::VertexBuffer VBO(true);
    KRE::ElementArray EBO(true);

    VAO.bind();
    VBO.bind();

    VBO.setData(vertices);

    EBO.bind();
    EBO.setData(indices);

    VBO.setVertexAttrib(0, 2, 4, 0);
    VBO.setVertexAttrib(1, 2, 4, 2);

    VBO.unbind();
    VAO.unbind();

    KRE::Shader shader;
    shader.compilePath("res/shaders/basicVertexShader.glsl", "res/shaders/basicFragmentShader.glsl");

    unsigned int outputImage;
    unsigned int dataImage;
    unsigned int textureWidth = SCREEN_WIDTH;
    unsigned int textureHeight = SCREEN_HEIGHT;
    createTexture(outputImage, textureWidth, textureHeight, 0);
    createTexture(dataImage, textureWidth, textureHeight, 1);

    KRE::CameraPerspective perspective = KRE::CameraPerspective::ORTHOGRAPHIC;
    KRE::CameraMovementTypes movement = KRE::CameraMovementTypes::LOCKED_PERSPECTIVE;
    KRE::Camera camera(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT), perspective, movement, glm::vec3(0.0f, 0.0f, 0.0f));

    glm::vec3 backgroundColour = glm::vec3(0.7f, 0.8f, 1.0f);

    ConstantData data;
    data.cameraUp = camera.up;
    data.cameraViewDist = 1.0f;
    data.cameraFocusDist = 10.0;
    data.cameraFov = 40.0f;
    data.cameraAperture = 0.0;
    data.maxDepth = 10;
    data.aspectRatio = ASPECT_RATIO;

    Scene scene;

    unsigned int currentChoice = 3;
    switch(currentChoice)
    {
    case 1:
        scene = randomScene();
        data.cameraPos = glm::vec3(13, 2, 3);
        data.cameraLookAt = glm::vec3(0, 0, 0);
        data.cameraFov = 20.0f;
        data.cameraAperture = 0.1f;
        break;
    case 2:
        scene = simpleLight();
        backgroundColour = glm::vec3(0.0f);
        data.cameraPos = glm::vec3(26, 3, 6);
        data.cameraLookAt = glm::vec3(0, 2, 0);
        data.cameraFov = 20.0f;
        break;
    case 3:
        scene = cornellBox();
        backgroundColour = glm::vec3(0.0f);
        // data.aspectRatio = 1.0f;
        data.cameraPos = glm::vec3(278, 278, -800);
        data.cameraLookAt = glm::vec3(278, 278, 0);
        data.cameraFov = 40.0f;
        break;
    default:
        backgroundColour = glm::vec3(0.0f);
        break;
    }

    std::vector<Shape>& sceneData = scene.getScene();
    std::cout << sceneData.size() << " : " << sizeof(Shape) * sceneData.size() << "\n";

    KRE::ComputeShader computeShader;
    computeShader.compilePath("res/shaders/BasicCompute.glsl");
    computeShader.bind();
    computeShader.setUniformVector3("u_BackgroundColour", backgroundColour);

    unsigned int sceneSSBO;
    glGenBuffers(1, &sceneSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sceneSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shape) * sceneData.size(), sceneData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, sceneSSBO);

    unsigned int dataSSBO;
    glGenBuffers(1, &dataSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ConstantData), &data, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, dataSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    shader.bind();
    shader.setUniformInt("u_Texture", 0);

    float sampleCount = 0;
    float maxSamples = 1000;

    while (!glfwWindowShouldClose(window))
    {
        KRE::Clock::tick();

        if (!(sampleCount >= maxSamples))
        {
            int localWorkGroupSize = 16;
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, sceneSSBO);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, dataSSBO);
            computeShader.bind();
            computeShader.setUniformFloat("u_SampleCount", sampleCount);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, outputImage);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, dataImage);

            glDispatchCompute(textureWidth / localWorkGroupSize, textureHeight / localWorkGroupSize, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            sampleCount += 1.0f;
            std::cout << "\r" << "Samples : " << (int)sampleCount;
        }


        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, outputImage);

        VAO.bind();
        shader.bind();

        glDrawElements(GL_TRIANGLES, indices.getCount(), GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void processKeys()
{

}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    switch (action)
    {
    case GLFW_PRESS: KRE::Keyboard::pressKey(key); break;
    case GLFW_RELEASE: KRE::Keyboard::unpressKey(key); break;
    }
}

void createTexture(unsigned int& id, int width, int height, int bindPoint)
{
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(bindPoint, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

float random()
{
    return (rand() % 1000) / 1000.0f;
}

Scene randomScene()
{
    Scene scene;

    Material groundMat = Lambertian(glm::vec3(0.5, 0.5, 0.5));
    scene.addShape(Sphere(glm::vec3(0, -1000, 0), 1000, groundMat));

    int maxSize = 7;

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
                    scene.addShape(Sphere(center, 0.2, mat));
                }
                else if (chooseMat < 0.95)
                {
                    glm::vec3 albedo = glm::linearRand(glm::vec3(0.5), glm::vec3(1.0));
                    float fuzz = random() / 0.5f;
                    mat = Metal(albedo, fuzz);
                    scene.addShape(Sphere(center, 0.2, mat));
                }
                else
                {
                    mat = Dielectric(1.5);
                    scene.addShape(Sphere(center, 0.2, mat));
                }
            }
        }
    }

    Material m1 = Dielectric(1.5);
    scene.addShape(Sphere(glm::vec3(0, 1, 0), 1.0, m1));

    Material m2 = Lambertian(glm::vec3(0.4, 0.2, 0.1));
    scene.addShape(Sphere(glm::vec3(-4, 1, 0), 1.0, m2));

    Material m3 = Metal(glm::vec3(0.7, 0.6, 0.5), 0.0);
    scene.addShape(Sphere(glm::vec3(4, 1, 0), 1.0, m3));

    return scene;
}

Scene simpleLight()
{
    Scene scene;

    // Material ground = Metal(glm::vec3(1.0, 1.0, 0.0), 0.9);
    Material ground = Lambertian(glm::vec3(1.0, 1.0, 0.0));
    Material center = Lambertian(glm::vec3(1.0, 0.0, 0.0));
    scene.addShape(Sphere(glm::vec3(0, -1000, 0), 1000, ground));
    scene.addShape(Sphere(glm::vec3(0, 2, 0), 2, center));

    Material light1 = DiffuseLight(glm::vec3(4));
    // scene.addShape(Sphere(glm::vec3(2, 2, -4), 0.5, light1));
    scene.addShape(XYRect(3, 5, 1, 3, -2, light1));

    // scene.addShape(Sphere(glm::vec3(0, 7, 0), 2, light1));

    return scene;
}

Scene cornellBox()
{
    Scene scene;

    Material red = Lambertian(glm::vec3(0.65, 0.05, 0.05));
    Material white = Lambertian(glm::vec3(0.73));
    Material green = Lambertian(glm::vec3(0.12, 0.45, 0.15));
    Material light = DiffuseLight(glm::vec3(15));

    scene.addShape(YZRect(0, 555, 0, 555, 555, green));
    scene.addShape(YZRect(0, 555, 0, 555, 0, red));
    scene.addShape(XZRect(213, 343, 227, 332, 554, light));
    scene.addShape(XZRect(0, 555, 0, 555, 0, white));
    scene.addShape(XZRect(0, 555, 0, 555, 555, white));
    scene.addShape(XYRect(0, 555, 0, 555, 555, white));

    return scene;
}