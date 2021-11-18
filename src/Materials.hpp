#ifndef MATERIALS_HPP
#define MATERIALS_HPP

#include <glm/glm.hpp>

enum MaterialType
{
    Mat_Lambertian = 0,
    Mat_Metal = 1,
    Mat_Dielectric = 2,
    Mat_DiffuseLight = 3
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

#endif