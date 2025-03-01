#ifndef TRIANGLE2D_CLASS_H
#define TRIANGLE2D_CLASS_H

#include "shape2D.h"

class Triangle2D : public Shape2D
{
private:
    float m_size;

public:
    Triangle2D(float size = 1.0f);

    // Initialize the triangle
    virtual void Initialize() override;

    // Set the size of the triangle
    void SetSize(float size);
    float GetSize() const { return m_size; }
};

#endif