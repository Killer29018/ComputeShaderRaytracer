#include "Scene.hpp"

std::vector<Shape>& Scene::getScene()
{
    return m_Scene;
}


void Scene::addShape(const Shape& shape)
{
    m_Scene.push_back(shape);
}