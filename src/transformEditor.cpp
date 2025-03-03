#include "transformEditor.h"
#include "triangle.h"
#include "rectangle.h"
#include "circle.h"
#include "polygon.h"

TransformEditor::TransformEditor()
    : m_selectedShape(nullptr), m_name("None")
{
}

void TransformEditor::SelectShape(std::shared_ptr<Shape2D> shape, const std::string &name)
{
    m_selectedShape = shape;
    m_name = name;
}

void TransformEditor::DeselectShape()
{
    m_selectedShape = nullptr;
    m_name = "None";
}

void TransformEditor::DrawImGuiControls()
{
    ImGui::Begin("Transform Editor");

    ImGui::Text("Selected: %s", m_name.c_str());

    if (m_selectedShape)
    {
        // Get current transform values
        glm::vec2 position = m_selectedShape->GetPosition();
        glm::vec2 scale = m_selectedShape->GetScale();
        float rotation = m_selectedShape->GetRotation();
        glm::vec3 color = m_selectedShape->GetColor();

        // Position controls
        ImGui::Text("Position");
        float pos[2] = {position.x, position.y};
        if (ImGui::DragFloat2("##Position", pos, 0.01f))
        {
            m_selectedShape->SetPosition(glm::vec2(pos[0], pos[1]));
        }

        // Scale controls
        ImGui::Text("Scale");
        float scl[2] = {scale.x, scale.y};
        if (ImGui::DragFloat2("##Scale", scl, 0.01f, 0.01f, 10.0f))
        {
            m_selectedShape->SetScale(glm::vec2(scl[0], scl[1]));
        }

        // Rotation controls
        ImGui::Text("Rotation");
        float rot = rotation;
        if (ImGui::DragFloat("##Rotation", &rot, 1.0f, 0.0f, 360.0f))
        {
            m_selectedShape->SetRotation(rot);
        }

        // Color controls
        ImGui::Text("Color");
        float col[3] = {color.r, color.g, color.b};
        if (ImGui::ColorEdit3("##Color", col))
        {
            m_selectedShape->SetColor(glm::vec3(col[0], col[1], col[2]));
        }

        // Type-specific controls
        if (auto triangle = std::dynamic_pointer_cast<Triangle2D>(m_selectedShape))
        {
            ImGui::Separator();
            ImGui::Text("Triangle Properties");
            float size = triangle->GetSize();
            if (ImGui::DragFloat("Size", &size, 0.01f, 0.01f, 10.0f))
            {
                triangle->SetSize(size);
            }
        }
        else if (auto rectangle = std::dynamic_pointer_cast<Rectangle2D>(m_selectedShape))
        {
            ImGui::Separator();
            ImGui::Text("Rectangle Properties");
            float width = rectangle->GetWidth();
            float height = rectangle->GetHeight();
            if (ImGui::DragFloat("Width", &width, 0.01f, 0.01f, 10.0f))
            {
                rectangle->SetSize(width, height);
            }
            if (ImGui::DragFloat("Height", &height, 0.01f, 0.01f, 10.0f))
            {
                rectangle->SetSize(width, height);
            }
        }
        else if (auto circle = std::dynamic_pointer_cast<Circle2D>(m_selectedShape))
        {
            ImGui::Separator();
            ImGui::Text("Circle Properties");
            float radius = circle->GetRadius();
            int segments = circle->GetSegments();
            if (ImGui::DragFloat("Radius", &radius, 0.01f, 0.01f, 10.0f))
            {
                circle->SetRadius(radius);
            }
            if (ImGui::SliderInt("Segments", &segments, 3, 100))
            {
                circle->SetSegments(segments);
            }
        }
        else if (auto polygon = std::dynamic_pointer_cast<Polygon2D>(m_selectedShape))
        {
            ImGui::Separator();
            ImGui::Text("Polygon Properties");
            ImGui::Text("Points: %zu", polygon->GetPoints().size());
            if (ImGui::Button("Clear Points"))
            {
                polygon->ClearPoints();
            }
            // Add point button
            static float newPoint[2] = {0.0f, 0.0f};
            ImGui::DragFloat2("New Point", newPoint, 0.01f, -10.0f, 10.0f);
            if (ImGui::Button("Add Point"))
            {
                polygon->AddPoint(glm::vec2(newPoint[0], newPoint[1]));
            }
        }
    }

    ImGui::End();
}

void TransformEditor::DrawInteractionMatrix(std::vector<std::vector<float>> &matrix, int numTypes,
                                            std::function<glm::vec3(int, int)> getColorFunc, const std::string &name, int min, int max)
{
    ImGui::Begin(name.c_str());

    if (matrix.empty() || matrix[0].empty())
    {
        ImGui::Text("Matrix is empty!");
        ImGui::End();
        return;
    }

    int rows = matrix.size();
    int cols = matrix[0].size();

    ImGui::Text("Matrix Size: %d x %d", rows, cols);

    float cellSize = 40.0f;        // Size for matrix cells
    float colorButtonSize = 20.0f; // Smaller size for color buttons

    ImGui::PushItemWidth(cellSize); // Set width for matrix cells

    // Top row (Column labels)
    ImGui::Text("   "); // Blank space for row header
    ImGui::SameLine();
    for (int j = 0; j < cols; ++j)
    {
        glm::vec3 color = getColorFunc(numTypes, j);
        // Use smaller color buttons for column labels
        ImGui::ColorButton(("##Col" + std::to_string(j)).c_str(), ImVec4(color.r, color.g, color.b, 1.0f), 0, ImVec2(cellSize, colorButtonSize / 2));
        ImGui::SameLine();
    }
    ImGui::NewLine();

    // Matrix values with row labels
    for (int i = 0; i < rows; ++i)
    {
        glm::vec3 rowColor = getColorFunc(numTypes, i);

        // Row label (color indicator)
        ImGui::ColorButton(("##Row" + std::to_string(i)).c_str(), ImVec4(rowColor.r, rowColor.g, rowColor.b, 1.0f), 0, ImVec2(colorButtonSize, colorButtonSize));
        ImGui::SameLine();

        // Cells
        for (int j = 0; j < cols; ++j)
        {
            std::string label = "##cell" + std::to_string(i) + "_" + std::to_string(j);
            ImGui::PushID(label.c_str());
            // Regular size for matrix cells
            ImGui::DragFloat("", &matrix[i][j], 0.01f, min, max, "%.2f");
            ImGui::PopID();
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }

    ImGui::PopItemWidth(); // Restore the default width
    ImGui::End();
}
