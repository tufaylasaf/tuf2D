#include "shape2D.h"

Shape2D::Shape2D()
    : m_position(0.0f, 0.0f), m_scale(1.0f, 1.0f), m_rotation(0.0f), m_color(1.0f, 1.0f, 1.0f), m_shader(nullptr)
{
}

Shape2D::~Shape2D()
{
    m_VAO.Delete();
    m_VBO.Delete();
    m_EBO.Delete();
}

void Shape2D::Draw()
{
    if (m_shader == nullptr)
        return;

    m_shader->Activate();

    // Set transformation matrix
    GLuint modelLoc = glGetUniformLocation(m_shader->ID, "model");
    glm::mat4 model = GetModelMatrix();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Set color
    GLuint colorLoc = glGetUniformLocation(m_shader->ID, "color");
    glUniform3fv(colorLoc, 1, glm::value_ptr(m_color));

    // Draw the shape
    m_VAO.Bind();
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    m_VAO.Unbind();
}

void Shape2D::SetPosition(const glm::vec2 &position)
{
    m_position = position;
}

void Shape2D::SetScale(const glm::vec2 &scale)
{
    m_scale = scale;
}

void Shape2D::SetRotation(float rotation)
{
    m_rotation = rotation;
}

void Shape2D::Translate(const glm::vec2 &translation)
{
    m_position += translation;
}

void Shape2D::Scale(const glm::vec2 &scale)
{
    m_scale *= scale;
}

void Shape2D::Rotate(float angle)
{
    m_rotation += angle;
}

void Shape2D::SetColor(const glm::vec3 &color)
{
    m_color = color;
}

Shader *Shape2D::GetShader()
{
    return m_shader;
}

void Shape2D::SetShader(Shader *shader)
{
    m_shader = shader;
}

glm::mat4 Shape2D::GetModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(m_position, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(m_scale, 1.0f));
    return model;
}

void Shape2D::SetupMesh()
{
    // Clean up any existing buffers
    if (m_VBO.ID != 0)
        m_VBO.Delete();
    if (m_EBO.ID != 0)
        m_EBO.Delete();

    // Create and bind VAO
    m_VAO.Bind();

    // Create and bind VBO, EBO
    m_VBO = VBO(m_vertices);
    m_EBO = EBO(m_indices);

    // Link attributes
    m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, position));
    m_VAO.LinkAttrib(m_VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    m_VAO.LinkAttrib(m_VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, texUV));

    // Unbind
    m_VAO.Unbind();
    m_VBO.Unbind();
    m_EBO.Unbind();
}