#ifndef SCENE_HPP
#define SCENE_HPP

#include <glm/glm.hpp>
#include <vector>

enum MaterialType
{
    Mat_Lambertian = 0,
    Mat_Metal = 1,
    Mat_Dielectric = 2
};

enum ShapeType
{
    Shape_Sphere = 0
};

struct Material
{
    MaterialType materialType;
    glm::vec3 colour;
    float extraInfo;

    Material() {}

    Material(MaterialType matType, glm::vec3 colour, float extraInfo)
        : materialType(matType), colour(colour), extraInfo(extraInfo) {}
};

struct Lambertian : public Material
{
    Lambertian(glm::vec3 colour)
        : Material(Mat_Lambertian, colour, 0.0f) {}
};

struct Metal : public Material
{
    Metal(glm::vec3 colour, float fuzzy)
        : Material(Mat_Metal, colour, fuzzy) {}
};

struct Dielectric : public Material
{
    Dielectric(float ir)
        : Material(Mat_Dielectric, glm::vec3(1.0), ir) {}
};

struct Shape
{
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 size;
    alignas(16) glm::vec3 colour;

    // ShapeType MatType MatExtra Throwaway
    alignas(16) glm::vec4 extraInfo;

    Shape(ShapeType type, glm::vec3 position, glm::vec3 size, MaterialType matType, glm::vec3 colour, float materialExtra)
        : position(position), size(size), colour(colour)
    {
        extraInfo.x = type;
        extraInfo.y = matType;
        extraInfo.z = materialExtra;
    }
};

struct Sphere : Shape
{
    Sphere(glm::vec3 position, float radius, const Material& mat)
        : Shape(Shape_Sphere, position, glm::vec3(radius), mat.materialType, mat.colour, mat.extraInfo) {}
};

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