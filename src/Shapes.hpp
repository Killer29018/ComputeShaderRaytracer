#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <glm/glm.hpp>

#include "Materials.hpp"

#include "iostream"

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

    // X Y Z Angle
    alignas(16) glm::vec4 rotation;

    // ShapeType MatType MatExtra constantMedium
    alignas(16) glm::vec4 extraInfo;

    Shape() = default;
    Shape(ShapeType type, glm::vec3 position, glm::vec3 size, MaterialType matType, glm::vec3 colour, float materialExtra, glm::vec4 rotation = glm::vec4(0.0f))
        : position(position), size(size), colour(colour)
    {
        extraInfo.x = type;
        extraInfo.y = matType;
        extraInfo.z = materialExtra;
        extraInfo.w = 0.0f;

        if (matType == Mat_ConstantMedium)
            extraInfo.w = 1.0;
    }

    void rotateX(float angle) { rotation = glm::vec4(1.0f, 0.0f, 0.0f, angle); }
    void rotateY(float angle) { rotation = glm::vec4(0.0f, 1.0f, 0.0f, angle); }
    void rotateZ(float angle) { rotation = glm::vec4(0.0f, 0.0f, 1.0f, angle); }
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
    Cube(glm::vec3 position, glm::vec3 size, const Material& mat, glm::vec3 rotation = glm::vec3(0.0f), float angle = 0.0f)
        : Shape(Shape_Cube, position, size, mat.materialType, mat.colour, mat.extraInfo, glm::vec4(rotation.x, rotation.y, rotation.z, angle)) {}
};


#endif