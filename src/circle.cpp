#include "circle.h"
#include <cmath>

Circle2D::Circle2D(float radius, int segments)
    : m_radius(radius), m_segments(segments)
{
    Initialize();
}

void Circle2D::Initialize()
{
    const float PI = 3.14159265358979323846f;

    // Clear previous data
    m_vertices.clear();
    m_indices.clear();

    // Center vertex
    Vertex center;
    center.position = glm::vec3(0.0f, 0.0f, 0.0f);
    center.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    center.texUV = glm::vec2(0.5f, 0.5f);
    m_vertices.push_back(center);

    // Outer vertices
    for (int i = 0; i < m_segments; ++i)
    {
        float angle = 2.0f * PI * i / m_segments;
        Vertex vertex;
        vertex.position = glm::vec3(m_radius * cos(angle), m_radius * sin(angle), 0.0f);
        vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
        vertex.texUV = glm::vec2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));

        m_vertices.push_back(vertex);
    }

    // Create triangle indices
    for (int i = 0; i < m_segments; ++i)
    {
        m_indices.push_back(0); // Center
        m_indices.push_back(1 + i);
        m_indices.push_back(1 + (i + 1) % m_segments);
    }

    // Setup the mesh
    SetupMesh();
}

void Circle2D::SetRadius(float radius)
{
    m_radius = radius;
    Initialize();
}

void Circle2D::SetSegments(int segments)
{
    m_segments = segments;
    Initialize();
}