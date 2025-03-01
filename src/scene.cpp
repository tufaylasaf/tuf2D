#include "scene.h"
#include <algorithm>

Scene2D::Scene2D()
    : m_defaultShader(nullptr)
{
}

Scene2D::~Scene2D()
{
    if (m_defaultShader)
    {
        m_defaultShader->Delete();
        delete m_defaultShader;
    }
}

void Scene2D::Initialize(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    // Create default shader
    m_defaultShader = new Shader(vertexShaderPath, fragmentShaderPath);
}

void Scene2D::AddShape(std::shared_ptr<Shape2D> shape)
{
    if (shape)
    {
        // If the shape doesn't have a shader, set the default one
        if (shape->GetShader() == nullptr)
        {
            shape->SetShader(m_defaultShader);
        }

        m_shapes.push_back(shape);
    }
}

void Scene2D::RemoveShape(std::shared_ptr<Shape2D> shape)
{
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end())
    {
        m_shapes.erase(it);
    }
}

void Scene2D::Draw()
{
    for (const auto &shape : m_shapes)
    {
        shape->Draw();
    }
}

void Scene2D::Clear()
{
    m_shapes.clear();
}