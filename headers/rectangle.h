#ifndef RECTANGLE2D_CLASS_H
#define RECTANGLE2D_CLASS_H

#include "shape2D.h"

class Rectangle2D : public Shape2D
{
private:
    float m_width;
    float m_height;

public:
    Rectangle2D(float width = 1.0f, float height = 1.0f);

    // Initialize the rectangle
    virtual void Initialize() override;

    // Set the size of the rectangle
    void SetSize(float width, float height);
    float GetWidth() const { return m_width; }
    float GetHeight() const { return m_height; }
};

#endif