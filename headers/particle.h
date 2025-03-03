#ifndef PARTICLE
#define PARTICLE

#include <glm/glm.hpp>
#include <vector>

class Particle
{
public:
    glm::vec2 pos;
    glm::vec2 vel;
    int type;

    Particle(glm::vec2 p, int t) : pos(p), vel(glm::vec2(0, 0)), type(t) {};

    void update(float dt, std::vector<Particle> &particles, std::vector<std::vector<float>> minDist, std::vector<std::vector<float>> forces, std::vector<std::vector<float>> radii, float w, float h);

private:
    float mapValue(float value, float inMin, float inMax, float outMin, float outMax);
};

#endif // !PARTICLE