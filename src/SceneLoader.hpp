#ifndef SCENE_LOADER_HPP
#define SCENE_LOADED_HPP

#include <vector>
#include "Shapes.hpp"
#include "Data.hpp"

#include "json/json.h"

class Scene;

class SceneLoader
{
public:
private:
    static std::vector<Shape> m_Scene;
    static ConstantData m_Data;

    static Json::Value m_Root;
public:
    static void loadFile(const char* filePath, Scene* scene);
private:
    SceneLoader() = default;

    static void getRoot(const char* filePath);
    static void loadData();
    static void loadShapes();

    static glm::vec3 getVec3(Json::Value& root);
    static ShapeType getShapeType(const char* value);
    static MaterialType getMaterialType(const char* value);
};

#endif