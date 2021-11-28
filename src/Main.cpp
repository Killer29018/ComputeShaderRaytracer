#include "Application.hpp"

int main(int argc, char* argv[])
{
    Application::init(glm::vec2(1280, 720));

    int data;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &data);
    std::cout << data << "\n";
    std::cout << 32768 * 32768 << " : " << 1920 * 1080 << " : " << 1280 * 720 << "\n";

    Application::run();

    return 0;
}