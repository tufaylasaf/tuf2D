#ifndef SHAPE2D_CLASS_H
#define SHAPE2D_CLASS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shader.h"

class Shape2D
{
protected:
    // Transformation properties
    glm::vec2 m_position;
    glm::vec2 m_scale;
    float m_rotation; // in degrees

    // OpenGL objects
    VAO m_VAO;
    VBO m_VBO;
    EBO m_EBO;

    // Vertices and indices
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    // Color
    glm::vec3 m_color;

    // Shader
    Shader *m_shader;

public:
    Shape2D();
    virtual ~Shape2D();

    // Initialize the shape - to be implemented by derived classes
    virtual void Initialize() = 0;

    // Draw the shape
    virtual void Draw();

    // Transform functions
    void SetPosition(const glm::vec2 &position);
    void SetScale(const glm::vec2 &scale);
    void SetRotation(float rotation);
    void Translate(const glm::vec2 &translation);
    void Scale(const glm::vec2 &scale);
    void Rotate(float angle);

    // Color functions
    void SetColor(const glm::vec3 &color);

    // Shader functions
    Shader *GetShader();
    void SetShader(Shader *shader);

    // Getters
    glm::vec2 GetPosition() const { return m_position; }
    glm::vec2 GetScale() const { return m_scale; }
    float GetRotation() const { return m_rotation; }
    glm::vec3 GetColor() const { return m_color; }

protected:
    // Update the model matrix
    glm::mat4 GetModelMatrix() const;

    // Setup VAO, VBO, EBO
    void SetupMesh();
};

#endif