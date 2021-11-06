#include <Glad/gl.h>
#include <GLFW/glfw3.h>

#include <KRE/KRE.hpp>
#include <GLM/glm.hpp>

#include <iostream>
#include <string>

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processKeys();

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

    float deltaTime = 0.0f;
    float lastTime = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        KRE::Clock::tick();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        VAO.bind();
        glDrawElements(GL_TRIANGLES, indices.getCount(), GL_UNSIGNED_INT, NULL);

        processKeys();
        glfwSwapBuffers(window);
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