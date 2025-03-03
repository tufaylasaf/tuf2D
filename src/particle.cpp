#include "particle.h"
#include <algorithm>
#include <glm/gtc/random.hpp>

float K = 0.075f;
float F = 0.85f;

void Particle::update(float dt, std::vector<Particle> &particles, std::vector<std::vector<float>> minDist,
                      std::vector<std::vector<float>> forces, std::vector<std::vector<float>> radii, float w, float h)
{
    glm::vec2 direction;
    glm::vec2 totalForce(0.0f, 0.0f);
    glm::vec2 acc(0.0f, 0.0f);
    float d;

    for (Particle &p : particles)
    {

        if (&p != this)
        {

            direction = p.pos - pos;

            if (direction.x > 0.5 * w)
            {
                direction.x -= w;
            }

            if (direction.x < -0.5 * w)
            {
                direction.x += w;
            }

            if (direction.y > 0.5 * h)
            {
                direction.y -= h;
            }

            if (direction.y < -0.5 * h)
            {
                direction.y += h;
            }

            d = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (d != 0)
            {
                direction /= d;
            }

            if (d < minDist[type][p.type])
            {

                glm::vec2 force = direction * std::abs(forces[type][p.type]) * -3.0f;

                force *= mapValue(d, 0, minDist[type][p.type], 1, 0) * K;

                totalForce += force;
            }
            if (d < radii[type][p.type])
            {
                glm::vec2 force = direction * forces[type][p.type];
                force *= mapValue(d, 0, radii[type][p.type], 1, 0) * K;
                totalForce += force;
            }
        }
    }

    acc += totalForce;

    vel += acc;

    pos += vel;

    pos.x = std::fmod(pos.x + w, w);
    pos.y = std::fmod(pos.y + h, h);

    vel *= F;
}

float Particle::mapValue(float value, float inMin, float inMax, float outMin, float outMax)
{
    // Map a value from one range to another
    return outMin + (outMax - outMin) * ((value - inMin) / (inMax - inMin));
}