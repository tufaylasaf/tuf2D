#ifndef SCENE2D_CLASS_H
#define SCENE2D_CLASS_H

#include <vector>
#include <memory>
#include "Shape2D.h"
#include "shader.h"

class Scene2D
{
private:
    std::vector<std::shared_ptr<Shape2D>> m_shapes;
    Shader *m_defaultShader;

public:
    Scene2D();
    ~Scene2D();

    // Initialize the scene with a default shader
    void Initialize(const char *vertexShaderPath, const char *fragmentShaderPath);

    // Add a shape to the scene
    template <typename T, typename... Args>
    std::shared_ptr<T> CreateShape(Args &&...args)
    {
        auto shape = std::make_shared<T>(std::forward<Args>(args)...);
        shape->SetShader(m_defaultShader);
        m_shapes.push_back(shape);
        return shape;
    }

    // Add an existing shape to the scene
    void AddShape(std::shared_ptr<Shape2D> shape);

    // Remove a shape from the scene
    void RemoveShape(std::shared_ptr<Shape2D> shape);

    // Draw all shapes in the scene
    void Draw();

    // Clear all shapes from the scene
    void Clear();

    // Get all shapes
    const std::vector<std::shared_ptr<Shape2D>> &GetShapes() const { return m_shapes; }
};

#endif