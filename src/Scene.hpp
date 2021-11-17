#ifndef SCENE_HPP
#define SCENE_HPP

#include <glm/glm.hpp>
#include <vector>

enum MaterialType
{
    Mat_Lambertian = 0,
    Mat_Metal = 1,
    Mat_Dielectric = 2,
    Mat_DiffuseLight = 3
};

enum ShapeType
{
    Shape_Sphere = 0,
    Shape_XYRect = 1,
    Shape_XZRect = 2,
    Shape_YZRect = 3
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

struct DiffuseLight : public Material
{
    DiffuseLight(glm::vec3 colour)
        : Material(Mat_DiffuseLight, colour, 0.0f) { }
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

struct XYRect : Shape
{
    XYRect(float x0, float x1, float y0, float y1, float k, const Material& mat)
        : Shape(Shape_XYRect, glm::vec3(x0, y0, k), glm::vec3(x1, y1, 0.0), mat.materialType, mat.colour, mat.extraInfo) {}
};

struct XZRect : Shape
{
    XZRect(float x0, float x1, float z0, float z1, float k, const Material& mat)
        : Shape(Shape_XZRect, glm::vec3(x0, k, z0), glm::vec3(x1, 0.0, z1), mat.materialType, mat.colour, mat.extraInfo) {}
};

struct YZRect : Shape
{
    YZRect(float y0, float y1, float z0, float z1, float k, const Material& mat)
        : Shape(Shape_YZRect, glm::vec3(k, y0, z0), glm::vec3(0.0, y1, z1), mat.materialType, mat.colour, mat.extraInfo) {}
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