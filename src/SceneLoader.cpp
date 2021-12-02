#include "SceneLoader.hpp"

#include "Scene.hpp"

std::vector<Shape> SceneLoader::m_Scene;
ConstantData SceneLoader::m_Data;
Json::Value SceneLoader::m_Root;

void SceneLoader::loadFile(const char* filePath, Scene* scene)
{
    getRoot(filePath);
    loadData();
    loadShapes();

    scene->setSceneAndData(m_Scene, m_Data);
}

void SceneLoader::getRoot(const char* filePath)
{
    std::ifstream file;
    file.open(filePath);

    Json::CharReaderBuilder builder;
    // builder["collectComments"] = true;

    JSONCPP_STRING errs;
    if (!parseFromStream(builder, file, &m_Root, &errs))
    {
        std::cerr << errs << "\n";
        exit(-1);
    }
}

#include <glm/gtx/string_cast.hpp>

void SceneLoader::loadData()
{
    Json::Value data = m_Root["Data"];
    Json::Value root;

    root = data["CameraPosition"];
    m_Data.cameraPos = getVec3(root);

    root = data["CameraLookAt"];
    m_Data.cameraLookAt = getVec3(root);

    root = data["CameraUp"];
    m_Data.cameraUp = getVec3(root);

    root = data["Background"];
    m_Data.background = getVec3(root);

    m_Data.cameraViewDist = data["CameraViewDist"].asFloat();
    m_Data.cameraFocusDist = data["CameraFocusDist"].asFloat();
    m_Data.cameraFov = data["CameraFOV"].asFloat();
    m_Data.cameraAperture = data["CameraAperture"].asFloat();
    m_Data.aspectRatio = 16.0 / 9.0;
}

void SceneLoader::loadShapes()
{
    m_Scene.clear();

    Json::Value shapes = m_Root["Shapes"];
    for (Json::Value shape : shapes)
    {
        Shape s;
        const char* shapeType = shape["ShapeType"].asCString();
        s.extraInfo[0] = (float)getShapeType(shapeType);

        const char* matType = shape["MaterialType"].asCString();
        s.extraInfo[1] = (float)getMaterialType(matType);

        s.extraInfo[2] = shape["MaterialExtra"].asFloat();

        s.extraInfo[3] = (s.extraInfo[1] == Mat_ConstantMedium);

        s.position = getVec3(shape["Position"]);
        s.size = getVec3(shape["Size"]);
        s.colour = getVec3(shape["Colour"]);

        float angle = shape["Angle"].asFloat();
        glm::vec3 rotation = getVec3(shape["Rotation"]);
        s.rotation = glm::vec4(rotation.x, rotation.y, rotation.z, angle);

        m_Scene.emplace_back(s);
    }
}

glm::vec3 SceneLoader::getVec3(Json::Value& root)
{
    glm::vec3 value;
    value.x = root[0].asFloat();
    value.y = root[1].asFloat();
    value.z = root[2].asFloat();

    return value;
}

ShapeType SceneLoader::getShapeType(const char* value)
{
    if (!strcmp(value, "SPHERE")) return Shape_Sphere;
    if (!strcmp(value, "XYRECT")) return Shape_XYRect;
    if (!strcmp(value, "XZRECT")) return Shape_XZRect;
    if (!strcmp(value, "YZRECT")) return Shape_YZRect;
    if (!strcmp(value, "CUBE")) return Shape_Cube;
}

MaterialType SceneLoader::getMaterialType(const char* value)
{
    if (!strcmp(value, "LAMBERTIAN")) return Mat_Lambertian;
    if (!strcmp(value, "METAL")) return Mat_Metal;
    if (!strcmp(value, "DIELECTRIC")) return Mat_Dielectric;
    if (!strcmp(value, "DIFFUSE_LIGHT")) return Mat_DiffuseLight;
    if (!strcmp(value, "CONSTANT_MEDIUM")) return Mat_ConstantMedium;
}