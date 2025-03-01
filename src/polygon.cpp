#include "polygon.h"
#include <algorithm>

Polygon2D::Polygon2D()
{
    // Default initialization with no points
}

Polygon2D::Polygon2D(const std::vector<glm::vec2> &points)
    : m_points(points)
{
    Initialize();
}

void Polygon2D::Initialize()
{
    // If we don't have at least 3 points, we can't create a polygon
    if (m_points.size() < 3)
        return;

    // Clear previous data
    m_vertices.clear();
    m_indices.clear();

    // Calculate center of the polygon
    glm::vec2 center(0.0f);
    for (const auto &point : m_points)
    {
        center += point;
    }
    center /= static_cast<float>(m_points.size());

    // Add center vertex
    Vertex centerVertex;
    centerVertex.position = glm::vec3(center, 0.0f);
    centerVertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    centerVertex.texUV = glm::vec2(0.5f, 0.5f);
    m_vertices.push_back(centerVertex);

    // Add outer vertices
    for (const auto &point : m_points)
    {
        Vertex vertex;
        vertex.position = glm::vec3(point, 0.0f);
        vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);

        // Calculate UV coordinates (this is simplified)
        glm::vec2 relativePos = (point - center) / 2.0f + glm::vec2(0.5f);
        vertex.texUV = relativePos;

        m_vertices.push_back(vertex);
    }

    // Create triangle fan indices
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        m_indices.push_back(0); // Center
        m_indices.push_back(1 + i);
        m_indices.push_back(1 + (i + 1) % m_points.size());
    }

    // Setup the mesh
    SetupMesh();
}

void Polygon2D::SetPoints(const std::vector<glm::vec2> &points)
{
    m_points = points;
    Initialize();
}

void Polygon2D::AddPoint(const glm::vec2 &point)
{
    m_points.push_back(point);
    Initialize();
}

void Polygon2D::ClearPoints()
{
    m_points.clear();
    m_vertices.clear();
    m_indices.clear();
}