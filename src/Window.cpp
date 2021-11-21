#include "Window.hpp"

#include <glm/gtc/random.hpp>

GLFWwindow* Window::window;
KRE::Camera Window::camera({0, 0});
ConstantData Window::data;
Scene Window::scene;
float Window::aspectRatio = 1.0;
float Window::maxSamples = 1000.0f;

KRE::ComputeShader Window::m_ComputeShader;
KRE::Shader Window::m_GeneralShader;
KRE::VertexArray Window::m_VAO;
glm::vec2 Window::m_WindowSize;

unsigned int Window::m_SceneSSBO;
unsigned int Window::m_DataSSBO;
unsigned int Window::m_OutputImage;
unsigned int Window::m_DataImage;

float Window::m_SampleCount = 0.0f;

void Window::init(glm::vec2 windowSize)
{
    m_WindowSize = windowSize;

    initGLFW();

    glfwSwapInterval(1);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cerr << "Failed to initialize OpenGL Context\n";
        exit(-1);
    }

    std::cout << "Loaded Opengl " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << "\n";

    glViewport(0, 0, m_WindowSize.x, m_WindowSize.y);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetKeyCallback(window, Window::KeyCallback);

    srand(time(0));

    camera = KRE::Camera(windowSize,
    KRE::CameraPerspective::PERSPECTIVE,
    KRE::CameraMovementTypes::LOCKED_PERSPECTIVE);

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

    m_GeneralShader.compilePath("res/shaders/basicVertexShader.glsl", "res/shaders/basicFragmentShader.glsl");
    m_GeneralShader.bind();
    m_GeneralShader.setUniformInt("u_Texture", 0);

    m_ComputeShader.compilePath("res/shaders/RaytracingCompute.glsl");

    createTexture(m_OutputImage, windowSize.x, windowSize.y, 0);
    createTexture(m_DataImage, windowSize.x, windowSize.y, 1);

    std::cout << m_OutputImage << ": " << m_DataImage << "\n";

    glGenBuffers(1, &m_SceneSSBO);
    glGenBuffers(1, &m_DataSSBO);

    resetData();
}

void Window::processKeys()
{

}

void Window::resetData()
{
    data.cameraPos = glm::vec3(0.0, 0.0, 1.0);
    data.cameraLookAt = glm::vec3(0.0, 0.0, 0.0);
    data.cameraUp = camera.up;
    data.background = glm::vec3(0.7, 0.8, 1.0);
    data.cameraViewDist = 1.0f;
    data.cameraFocusDist = 10.0;
    data.cameraFov = 40.0f;
    data.cameraAperture = 0.0;
    data.maxDepth = 10;
    data.aspectRatio = aspectRatio;
}

void Window::run()
{
    std::cout << m_OutputImage << ": " << m_DataImage << "\n";
    while (!glfwWindowShouldClose(window))
    {
        KRE::Clock::tick();

        if (!(m_SampleCount >= maxSamples))
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

            glDispatchCompute(m_WindowSize.x / localWorkGroupSize, m_WindowSize.y / localWorkGroupSize, 1);
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

        const unsigned int INDICES_COUNT = 6;
        glDrawElements(GL_TRIANGLES, INDICES_COUNT, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Window::changeScene(SceneType sceneOption)
{
    switch(sceneOption)
    {
    case Scene_RandomSpheres:
        scene = randomScene();
        data.cameraPos = glm::vec3(13, 2, 3);
        data.cameraLookAt = glm::vec3(0, 0, 0);
        data.cameraFov = 20.0f;
        data.cameraAperture = 0.1f;
        break;
    case Scene_Lighting:
        scene = simpleLight();
        data.background = glm::vec3(0.0f);
        data.cameraPos = glm::vec3(26, 3, 6);
        data.cameraLookAt = glm::vec3(0, 2, 0);
        data.cameraFov = 20.0f;
        break;
    case Scene_CornellBox:
        scene = cornellBox();
        data.background = glm::vec3(0.0f);
        // data.aspectRatio = 1.0f;
        data.cameraPos = glm::vec3(278, 278, -800);
        data.cameraLookAt = glm::vec3(278, 278, 0);
        data.cameraFov = 40.0f;
        break;
    default:
        data.background = glm::vec3(0.0f);
        break;
    }
}

void Window::uploadDataToCompute()
{
    std::vector<Shape>& sceneData = scene.getScene();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SceneSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shape) * sceneData.size(), sceneData.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DataSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ConstantData), &data, GL_DYNAMIC_DRAW);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    switch (action)
    {
    case GLFW_PRESS: KRE::Keyboard::pressKey(key); break;
    case GLFW_RELEASE: KRE::Keyboard::unpressKey(key); break;
    }
}

void Window::initGLFW()
{
    if (!glfwInit())
        exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y, "Raytracing", NULL, NULL);

    if (!window)
    {
        std::cerr << "Failed to create Window\n";
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
}

void Window::createTexture(unsigned int& id, int width, int height, int bindPort)
{
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(bindPort, id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

float random()
{
    return (rand() % 1000) / 1000.0f;
}

Scene Window::randomScene()
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

Scene Window::simpleLight()
{
    Scene scene;

    // Material ground = Metal(glm::vec3(1.0, 1.0, 0.0), 0.9);
    Material ground = Lambertian(glm::vec3(1.0, 1.0, 0.0));
    Material center = Lambertian(glm::vec3(1.0, 0.0, 0.0));
    scene.addShape(Sphere(glm::vec3(0, -1000, 0), 1000, ground));
    scene.addShape(Sphere(glm::vec3(0, 2, 0), 2, center));

    Material light1 = DiffuseLight(glm::vec3(4));
    // scene.addShape(Sphere(glm::vec3(2, 2, -4), 0.5, light1));
    scene.addShape(XYRect(3, 2, 1, 2, -2, light1));

    // scene.addShape(Sphere(glm::vec3(0, 7, 0), 2, light1));

    return scene;
}

Scene Window::cornellBox()
{
    Scene scene;

    Material red = Lambertian(glm::vec3(0.65, 0.05, 0.05));
    Material white = Lambertian(glm::vec3(0.73));
    Material green = Lambertian(glm::vec3(0.12, 0.45, 0.15));
    Material light = DiffuseLight(glm::vec3(15));

    scene.addShape(YZRect(0, 555, 0, 555, 555, green));
    scene.addShape(YZRect(0, 555, 0, 555, 0, red));
    scene.addShape(XZRect(213, 130, 227, 105, 554, light));
    scene.addShape(XZRect(0, 555, 0, 555, 0, white));
    scene.addShape(XZRect(0, 555, 0, 555, 555, white));
    scene.addShape(XYRect(0, 555, 0, 555, 555, white));

    scene.addShape(Cube(glm::vec3(130, 0, 65), glm::vec3(165, 165, 165), white));
    scene.addShape(Cube(glm::vec3(265, 0, 295), glm::vec3(165, 330, 165), white));
    // scene.addShape(Cube(glm::vec3(320, 450, 65), glm::vec3(165, 165, 165), white));

    return scene;
}