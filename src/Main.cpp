#include <Glad/gl.h>
#include <GLFW/glfw3.h>

#include <KRE/KRE.hpp>
#include <GLM/glm.hpp>

#include <iostream>
#include <string>

#include <glm/gtx/string_cast.hpp>

#include "Scene.hpp"

static const float ASPECT_RATIO = 16.0/9.0;
static const unsigned int SCREEN_WIDTH = 1280;
static const unsigned int SCREEN_HEIGHT = SCREEN_WIDTH / ASPECT_RATIO;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processKeys();

void createTexture(unsigned int& id, int width, int height);

struct ConstantData
{
    alignas(16) glm::vec3 cameraPos;
    alignas(16) glm::vec3 cameraLookAt;
    alignas(16) glm::vec3 cameraUp;
    float cameraViewDist;
    float cameraFocusDist;
    float cameraFov;
    float cameraAperture;
    unsigned int imageSampling;
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

    glfwSwapInterval(0);

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

    unsigned int texture;
    unsigned int textureWidth = SCREEN_WIDTH;
    unsigned int textureHeight = SCREEN_HEIGHT;
    createTexture(texture, textureWidth, textureHeight);

    KRE::CameraPerspective perspective = KRE::CameraPerspective::ORTHOGRAPHIC;
    KRE::CameraMovementTypes movement = KRE::CameraMovementTypes::LOCKED_PERSPECTIVE;
    KRE::Camera camera(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT), perspective, movement, glm::vec3(0.0f, 0.0f, 0.0f));

    std::cout << "sX: " << SCREEN_WIDTH << " sY: " << SCREEN_HEIGHT << "\n";
    std::cout << "Aspect Ratio: " << ASPECT_RATIO << "\n";

    Material matGround = Lambertian(glm::vec3(0.8, 0.8, 0.0));
    Shape ground = Sphere(glm::vec3(0.0, -100.5, -1.0), 100, matGround);

    Material matCen = Lambertian(glm::vec3(0.1, 0.2, 0.5));
    Shape center = Sphere(glm::vec3(0.0, 0.0, -1.0), 0.5, matCen);

    Material matLeft = Dielectric(1.5);
    Shape left1 = Sphere(glm::vec3(-1.0, 0.0, -1.0), 0.5, matLeft);
    Shape left2 = Sphere(glm::vec3(-1.0, 0.0, -1.0), -0.45, matLeft);

    Material matRight = Metal(glm::vec3(0.8, 0.6, 0.2), 0.0);
    Shape right = Sphere(glm::vec3(1.0, 0.0, -1.0), 0.5, matRight);


    Scene scene;
    scene.addShape(ground);
    scene.addShape(center);
    scene.addShape(left1);
    scene.addShape(left2);
    scene.addShape(right);

    std::vector<glm::vec4>& sceneData = scene.getScene();

    for (glm::vec4 v : sceneData)
    {
        std::cout << glm::to_string(v) << "\n";
    }
    // glm::vec4 sceneData[] = {
    //     // Header
    //         // Shapes, Data, Material
    //     glm::vec4(5.0, 5.0, 4.0, 3.0),
    //     // Shapes
    //         // shape_type | shape_data_offset | material_data_offset | material_type
    //     glm::vec4(0.0, 0.0, 0.0, 0.0),
    //     glm::vec4(0.0, 1.0, 1.0, 0.0),
    //     glm::vec4(0.0, 2.0, 2.0, 2.0),
    //     glm::vec4(0.0, 3.0, 2.0, 2.0),
    //     glm::vec4(0.0, 4.0, 3.0, 1.0),
    //     // Shape Data
    //         // Circle
    //             // [x, y, z], r
    //     glm::vec4( 0.0,  -100.5, -1.0,   100),
    //     glm::vec4( 0.0,     0.0, -1.0,   0.5),
    //     glm::vec4(-1.0,     0.0, -1.0,   0.5),
    //     glm::vec4(-1.0,     0.0, -1.0, -0.45),
    //     glm::vec4( 1.0,     0.0, -1.0,   0.5),
    //     // Material Data
    //         // [r, g, b] material_extra
    //     glm::vec4(0.8, 0.8, 0.0, 0.0),
    //     glm::vec4(0.1, 0.2, 0.5, 0.0),
    //     glm::vec4(0.0, 0.0, 0.0, 1.5),
    //     glm::vec4(0.8, 0.6, 0.2, 0.0),

    //     // Extra
    //     glm::vec4(0, 0, 0, 0),
    //     glm::vec4(1, 0, 0, 0),
    //     glm::vec4(2, 0, 0, 0),
    // };

    camera.position = glm::vec3(0.0f, 0.0f, 0.0f);

    camera.front = glm::vec3(0, 0, -1.0);
    glm::vec3 cameraLookAt = camera.position + camera.front;

    const float viewportHeight = 9;
    const float viewportWidth = viewportHeight * ASPECT_RATIO;

    camera.position = glm::vec3(3, 3, 7);
    cameraLookAt = glm::vec3(0, 0, -1);

    ConstantData data;
    data.cameraPos = camera.position;
    data.cameraLookAt = cameraLookAt;
    data.cameraUp = camera.up;
    data.cameraViewDist = 1.0f;
    data.cameraFocusDist = 10.0;
    data.cameraFov = 20.0f;
    data.cameraAperture = 0.1;
    data.imageSampling = 50;
    data.maxDepth = 50;
    data.aspectRatio = ASPECT_RATIO;

    KRE::ComputeShader computeShader;
    computeShader.compilePath("res/shaders/BasicCompute.glsl");
    computeShader.bind();

    std::cout << sizeof(glm::vec4) << ": " << sizeof(glm::vec4) * sceneData.size() << "\n";

    unsigned int sceneSSBO;
    glGenBuffers(1, &sceneSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sceneSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * sceneData.size(), sceneData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sceneSSBO);

    unsigned int dataSSBO;
    glGenBuffers(1, &dataSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ConstantData), &data, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, dataSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    shader.bind();
    shader.setUniformInt("u_Texture", 0);

    while (!glfwWindowShouldClose(window))
    {
        KRE::Clock::tick();

        {
            int localWorkGroupSize = 16;
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sceneSSBO);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, dataSSBO);
            computeShader.bind();
            glDispatchCompute(textureWidth / localWorkGroupSize, textureHeight / localWorkGroupSize, 1);
        }

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        {
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            VAO.bind();
            shader.bind();

            glDrawElements(GL_TRIANGLES, indices.getCount(), GL_UNSIGNED_INT, NULL);
        }

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

void createTexture(unsigned int& id, int width, int height)
{
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}