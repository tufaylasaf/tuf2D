#ifndef POLYGON2D_CLASS_H
#define POLYGON2D_CLASS_H

#include "shape2D.h"

class Polygon2D : public Shape2D
{
private:
    std::vector<glm::vec2> m_points;

public:
    Polygon2D();
    Polygon2D(const std::vector<glm::vec2> &points);

    // Initialize the polygon
    virtual void Initialize() override;

    // Set the points of the polygon
    void SetPoints(const std::vector<glm::vec2> &points);
    const std::vector<glm::vec2> &GetPoints() const { return m_points; }

    // Add a point to the polygon
    void AddPoint(const glm::vec2 &point);

    // Clear all points
    void ClearPoints();
};

#endif