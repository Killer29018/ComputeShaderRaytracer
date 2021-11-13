#include "Scene.hpp"

std::vector<glm::vec4>& Scene::getScene()
{
    return (!m_Updated) ? m_Scene : createScene();
}

std::vector<glm::vec4>& Scene::createScene()
{
    m_Updated = false;

    m_Scene.clear();

    glm::vec4 value;

    // Header
    value = glm::vec4(m_ShapeCount, m_ShapeDataCount, m_MaterialCount, m_ExtraCount);
    unsigned int totalSize = m_ShapeCount + m_ShapeDataCount + m_MaterialCount + m_ExtraCount + 1;
    m_Scene.resize(totalSize);
    m_Scene.at(0) = value;

    unsigned int shapeOffset = 1;
    unsigned int shapeDataOffset = m_ShapeCount + 1;
    unsigned int materialOffset = m_ShapeCount + m_ShapeDataCount + 1;
    unsigned int extraOffset = m_ShapeCount + m_ShapeDataCount + m_MaterialCount + 1;

    for (int i = 0; i < m_Shapes.size(); i++)
    {
        unsigned int type = (unsigned int)m_Shapes[i].type;
        value = glm::vec4(type, i, i, i);
        m_Scene.at(shapeOffset + i) = value;

        switch (type)
        {
        case ((unsigned int)ShapeType::Sphere):
            glm::vec3 pos = m_Shapes[i].position;
            float radius = m_Shapes[i].radius;
            value = glm::vec4(pos.x, pos.y, pos.z, radius);
            m_Scene.at(shapeDataOffset + i) = value;

            unsigned int matType = (unsigned int)m_Shapes[i].mat.materialType;
            unsigned int extraCount = 0;

            value = glm::vec4(matType, extraCount, 0.0, 0.0);
            m_Scene.at(extraOffset + i) = value;

            break;
        }

        glm::vec3 colour = m_Shapes[i].mat.colour;
        float extra = m_Shapes[i].mat.extraInfo;

        value = glm::vec4(colour.r, colour.g, colour.b, extra);
        m_Scene.at(materialOffset + i) = value;
    }

    return m_Scene;
}

void Scene::addShape(Shape& shape)
{
    m_Updated = true;

    m_ShapeCount++;
    m_ShapeDataCount++;
    m_MaterialCount++;

    switch (shape.type)
    {
    case ShapeType::Sphere: m_ExtraCount += 1; break;
    }

    m_Shapes.push_back(shape);
}

void Scene::addMaterial(Material& material)
{
    return;
}

void Scene::addElementToScene(unsigned int& offset, glm::vec4& value)
{
    m_Scene.insert(m_Scene.begin() + (offset++), value);
}