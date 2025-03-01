#ifndef CIRCLE2D_CLASS_H
#define CIRCLE2D_CLASS_H

#include "Shape2D.h"

class Circle2D : public Shape2D
{
private:
    float m_radius;
    int m_segments;

public:
    Circle2D(float radius = 0.5f, int segments = 32);

    // Initialize the circle
    virtual void Initialize() override;

    // Set the size and resolution of the circle
    void SetRadius(float radius);
    void SetSegments(int segments);

    float GetRadius() const { return m_radius; }
    int GetSegments() const { return m_segments; }
};

#endif