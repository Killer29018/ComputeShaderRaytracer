#include <Glad/gl.h>
#include <GLFW/glfw3.h>

#include <KRE/KRE.hpp>
#include <GLM/glm.hpp>

#include <iostream>
#include <string>

static const float ASPECT_RATIO = 16.0/9.0;
static const unsigned int SCREEN_WIDTH = 1280;
static const unsigned int SCREEN_HEIGHT = SCREEN_WIDTH / ASPECT_RATIO;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processKeys();

void createTexture(unsigned int& id, int width, int height);

struct ShapeData
{
    glm::vec3 position;
    glm::vec3 size;
    float materialIndex;
};

struct CameraData
{
    float cameraPos[3];
    float cameraLookAt[3];
    float cameraUp[3];
    float cameraFocusDist;
    float cameraFov;
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
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f
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
    // unsigned int textureWidth = SCREEN_WIDTH;
    // unsigned int textureHeight = SCREEN_HEIGHT;
    unsigned int textureWidth = 400;
    unsigned int textureHeight = 225;
    createTexture(texture, textureWidth, textureHeight);

    KRE::CameraPerspective perspective = KRE::CameraPerspective::ORTHOGRAPHIC;
    KRE::CameraMovementTypes movement = KRE::CameraMovementTypes::LOCKED_PERSPECTIVE;
    KRE::Camera camera(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT), perspective, movement, glm::vec3(0.0f, 0.0f, 0.0f));

    std::cout << "sX: " << SCREEN_WIDTH << " sY: " << SCREEN_HEIGHT << "\n";
    std::cout << "Aspect Ratio: " << ASPECT_RATIO << "\n";

    glm::vec4 sceneData[] = {
        // Header
            // Shapes, Data, Material
        glm::vec4(2.0, 2.0, 2.0, 0.0),
        // Shapes
            // [i] shape_type | shape_data_offset | material_data_offset
        glm::vec4(0.0, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 1.0, 2.0, 0.0),
        // Shape Data
            // Circle
                // [x, y, z], r
        glm::vec4(0.0, 0.0, -1.0, 0.5),
        glm::vec4(0.0, -50.5, 0.0, 50),
        // Material Data
            // [r, g, b] Material Type
        glm::vec4(1.0, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 1.0, 0.0, 0.0)
    };

    float cameraFocusDist = 10.0f;
    float cameraFOV = camera.zoom;

    camera.position = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::vec3 cameraLookAt = camera.position + camera.front;

    CameraData cameraData;
    memcpy(&cameraData.cameraPos, &camera.position.x, sizeof(float) * 3);
    memcpy(&cameraData.cameraLookAt, &cameraLookAt.x, sizeof(float) * 3);
    memcpy(&cameraData.cameraUp, &camera.up.x, sizeof(float) * 3);
    cameraData.cameraFocusDist = 10.0f;
    cameraData.cameraFov = camera.zoom;

    std::cout << cameraData.cameraPos[0] << " : " << cameraData.cameraPos[1] << " : " << cameraData.cameraPos[2] << "\n";

    KRE::ComputeShader computeShader;
    computeShader.compilePath("res/shaders/BasicCompute.glsl");
    computeShader.bind();

    computeShader.setUniformInt("u_ImageWidth", SCREEN_WIDTH);
    computeShader.setUniformInt("u_ImageHeight", SCREEN_HEIGHT);

    const float viewportHeight = 2;
    const float viewportWidth = viewportHeight * ASPECT_RATIO;

    computeShader.setUniformFloat("u_ViewportWidth", viewportWidth);
    computeShader.setUniformFloat("u_ViewportHeight", viewportHeight);
    computeShader.setUniformFloat("u_AspectRatio", ASPECT_RATIO);

    unsigned int sceneSSBO;
    glGenBuffers(1, &sceneSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sceneSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(sceneData), &sceneData, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sceneSSBO);

    unsigned int cameraSSBO;
    glGenBuffers(1, &cameraSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, cameraSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(cameraData), &cameraData, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, sceneSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    shader.bind();
    shader.setUniformInt("u_Texture", 0);

    while (!glfwWindowShouldClose(window))
    {
        KRE::Clock::tick();

        {
            int localWorkGroupSize = 16;
            // glBindBuffer(GL_SHADER_STORAGE_BUFFER, sceneData);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sceneSSBO);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, cameraSSBO);
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