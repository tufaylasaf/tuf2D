#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera2D
{
private:
    glm::mat4 m_projectionMatrix;
    float m_aspectRatio;

public:
    Camera2D() : m_aspectRatio(1.0f)
    {
        m_projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    }

    void UpdateAspectRatio(float width, float height)
    {
        m_aspectRatio = width / height;

        if (m_aspectRatio >= 1.0f)
        {
            // Wider than tall
            m_projectionMatrix = glm::ortho(-m_aspectRatio, m_aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        }
        else
        {
            // Taller than wide
            m_projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f / m_aspectRatio, 1.0f / m_aspectRatio, -1.0f, 1.0f);
        }
    }

    const glm::mat4 &GetProjectionMatrix() const
    {
        return m_projectionMatrix;
    }
};