#include "triangle.h"

Triangle2D::Triangle2D(float size)
    : m_size(size)
{
    Initialize();
}

void Triangle2D::Initialize()
{
    float halfSize = m_size / 2.0f;

    // Clear previous data
    m_vertices.clear();
    m_indices.clear();

    // Define the vertices (equilateral triangle)
    Vertex v1, v2, v3;

    // Top vertex
    v1.position = glm::vec3(0.0f, halfSize, 0.0f);
    v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v1.texUV = glm::vec2(0.5f, 1.0f);

    // Bottom left vertex
    v2.position = glm::vec3(-halfSize, -halfSize, 0.0f);
    v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v2.texUV = glm::vec2(0.0f, 0.0f);

    // Bottom right vertex
    v3.position = glm::vec3(halfSize, -halfSize, 0.0f);
    v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v3.texUV = glm::vec2(1.0f, 0.0f);

    // Add vertices to the vector
    m_vertices.push_back(v1);
    m_vertices.push_back(v2);
    m_vertices.push_back(v3);

    // Define indices (just one triangle)
    m_indices = {0, 1, 2};

    // Setup the mesh
    SetupMesh();
}

void Triangle2D::SetSize(float size)
{
    m_size = size;
    Initialize();
}