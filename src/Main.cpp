#include "Application.hpp"

int main(int argc, char* argv[])
{
    Application::init(glm::vec2(1280, 720));
    Application::run();

    return 0;
}