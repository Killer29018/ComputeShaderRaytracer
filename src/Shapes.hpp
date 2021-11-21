#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <glm/glm.hpp>

#include "Materials.hpp"

enum ShapeType
{
    Shape_Sphere = 0,
    Shape_XYRect = 1,
    Shape_XZRect = 2,
    Shape_YZRect = 3,
    Shape_Cube = 4
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
    XYRect(float x, float sizeX, float y, float sizeY, float k, const Material& mat)
        : Shape(Shape_XYRect, glm::vec3(x, y, k), glm::vec3(sizeX, sizeY, 0.0), mat.materialType, mat.colour, mat.extraInfo) {}
};

struct XZRect : Shape
{
    XZRect(float x, float sizeX, float z, float sizeZ, float k, const Material& mat)
        : Shape(Shape_XZRect, glm::vec3(x, k, z), glm::vec3(sizeX, 0.0, sizeZ), mat.materialType, mat.colour, mat.extraInfo) {}
};

struct YZRect : Shape
{
    YZRect(float y, float sizeY, float z, float sizeZ, float k, const Material& mat)
        : Shape(Shape_YZRect, glm::vec3(k, y, z), glm::vec3(0.0, sizeY, sizeZ), mat.materialType, mat.colour, mat.extraInfo) {}
};

struct Cube : Shape
{
    Cube(glm::vec3 position, glm::vec3 size, const Material& mat)
        : Shape(Shape_Cube, position, size, mat.materialType, mat.colour, mat.extraInfo) {}
};

#endif