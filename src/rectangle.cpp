#include "rectangle.h"

Rectangle2D::Rectangle2D(float width, float height)
    : m_width(width), m_height(height)
{
    Initialize();
}

void Rectangle2D::Initialize()
{
    float halfWidth = m_width / 2.0f;
    float halfHeight = m_height / 2.0f;

    // Clear previous data
    m_vertices.clear();
    m_indices.clear();

    // Define the vertices
    Vertex v1, v2, v3, v4;

    // Top left
    v1.position = glm::vec3(-halfWidth, halfHeight, 0.0f);
    v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v1.texUV = glm::vec2(0.0f, 1.0f);

    // Top right
    v2.position = glm::vec3(halfWidth, halfHeight, 0.0f);
    v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v2.texUV = glm::vec2(1.0f, 1.0f);

    // Bottom right
    v3.position = glm::vec3(halfWidth, -halfHeight, 0.0f);
    v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v3.texUV = glm::vec2(1.0f, 0.0f);

    // Bottom left
    v4.position = glm::vec3(-halfWidth, -halfHeight, 0.0f);
    v4.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v4.texUV = glm::vec2(0.0f, 0.0f);

    // Add vertices to the vector
    m_vertices.push_back(v1);
    m_vertices.push_back(v2);
    m_vertices.push_back(v3);
    m_vertices.push_back(v4);

    // Define indices for two triangles
    m_indices = {0, 1, 2, 2, 3, 0};

    // Setup the mesh
    SetupMesh();
}

void Rectangle2D::SetSize(float width, float height)
{
    m_width = width;
    m_height = height;
    Initialize();
}