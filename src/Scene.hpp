#ifndef SCENE_HPP
#define SCENE_HPP

#include <glm/glm.hpp>
#include <vector>

#include "Materials.hpp"
#include "Shapes.hpp"

class Scene
{
public:
private:
    std::vector<Shape> m_Scene;
public:
    Scene() = default;
    ~Scene() = default;

    std::vector<Shape>& getScene();

    void addShape(const Shape& shape);
private:
};

#endif