#ifndef DATA_HPP
#define DATA_HPP

#include <glm/glm.hpp>

struct ConstantData
{
    alignas(16) glm::vec3 cameraPos;
    alignas(16) glm::vec3 cameraLookAt;
    alignas(16) glm::vec3 cameraUp;
    alignas(16) glm::vec3 background;
    float cameraViewDist;
    float cameraFocusDist;
    float cameraFov;
    float cameraAperture;
    unsigned int maxDepth;
    float aspectRatio;
};

#endif