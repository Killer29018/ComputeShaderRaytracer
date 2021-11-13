#ifndef SCENE_HPP
#define SCENE_HPP

#include <glm/glm.hpp>
#include <vector>

enum class MaterialType
{
    Lambertian = 0,
    Metal = 1,
    Dielectric = 2
};

enum class ShapeType
{
    Sphere = 0
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
        : Material(MaterialType::Lambertian, colour, 0.0f) {}
};

struct Metal : public Material
{
    Metal(glm::vec3 colour, float fuzzy)
        : Material(MaterialType::Metal, colour, fuzzy) {}
};

struct Dielectric : public Material
{
    Dielectric(float ir)
        : Material(MaterialType::Dielectric, glm::vec3(1.0), ir) {}
};

struct Shape
{
    glm::vec3 position;
    unsigned int materialIndex;
    ShapeType type;

    float radius;

    Shape(glm::vec3 position, unsigned int mat, ShapeType type)
        : position(position), materialIndex(mat), type(type) {}
};

struct Sphere : Shape
{
    Sphere(glm::vec3 position, float radius, unsigned int mat)
        : Shape(position, mat, ShapeType::Sphere)
    {
        this->radius = radius;
    }
};

class Scene
{
public:
private:
    std::vector<Shape> m_Shapes;
    std::vector<Material> m_Materials;

    bool m_Updated = false;
    std::vector<glm::vec4> m_Scene;

    float m_ShapeCount = 0;
    float m_ShapeDataCount = 0;
    float m_MaterialCount = 0;
    float m_ExtraCount = 0;
public:
    Scene() = default;
    ~Scene() = default;

    std::vector<glm::vec4>& getScene();
    std::vector<glm::vec4>& createScene();

    void addShape(const Shape& shape);
    unsigned int addMaterial(const Material& material);
private:
    void addElementToScene(unsigned int& offset, glm::vec4& value);
};

#endif