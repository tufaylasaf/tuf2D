#ifndef TRANSFORM_EDITOR_H
#define TRANSFORM_EDITOR_H

#include <imgui.h>
#include <memory>
#include "shape2D.h"
#include <functional>

class TransformEditor
{
private:
    std::shared_ptr<Shape2D> m_selectedShape;
    std::string m_name;

public:
    TransformEditor();

    // Select a shape to edit
    void SelectShape(std::shared_ptr<Shape2D> shape, const std::string &name);

    // Deselect the current shape
    void DeselectShape();

    // Draw the ImGui controls
    void DrawImGuiControls();
    void DrawInteractionMatrix(std::vector<std::vector<float>> &matrix, int numTypes,
                               std::function<glm::vec3(int, int)> getColorFunc, const std::string &name, int min, int max);

    // Get the currently selected shape
    std::shared_ptr<Shape2D> GetSelectedShape() const
    {
        return m_selectedShape;
    }
};

#endif