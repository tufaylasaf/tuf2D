#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera2D
{
private:
    glm::mat4 m_projectionMatrix;
    float m_width, m_height;

public:
    Camera2D() : m_width(1600.0f), m_height(900.0f)
    {
        UpdateProjection();
    }

    void UpdateAspectRatio(float width, float height)
    {
        m_width = width;
        m_height = height;
        UpdateProjection();
    }

    void UpdateProjection()
    {
        // Top-left origin: (0,0) at top-left, (width,height) at bottom-right
        m_projectionMatrix = glm::ortho(0.0f, m_width, m_height, 0.0f, -1.0f, 1.0f);
    }

    const glm::mat4 &GetProjectionMatrix() const
    {
        return m_projectionMatrix;
    }
};
