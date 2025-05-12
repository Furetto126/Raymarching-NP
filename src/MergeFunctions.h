#pragma once

#include <glm/vec3.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/rotate_vector.hpp>

template<typename T>
inline T easeIn(T a, T b, float c)
{
        return glm::mix(a, b, c*c*c*c*c);
}

template<typename T>
inline T easeOut(T a, T b, float c)
{
        float p = 1.0f - exp(-c);
        return glm::mix(a, b, p);
}

template<typename T>
inline T easeIn(const T &a, const T &b, float c)
{
        return glm::mix(a, b, c*c*c*c*c);
}

template<typename T>
inline T easeOut(const T &a, const T &b, float c)
{
        float p = 1.0f - exp(-c);
        return glm::mix(a, b, p);
}

inline float fakeGaussian(float x)
{
        float steepness = 3.0;
        float f1 = glm::smoothstep(0.0f, 1.0f, pow((x+0.5f), steepness));
        float f2 = glm::smoothstep(0.0f, 1.0f, pow((-x+1.5f), steepness));

        return fmin(f1, f2);
}

inline glm::vec3 coplanarCircle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &n, float c)
{
        if (a == b)
                return a;

        if (c == 0.0f)
                return a;

        if (c == 1.0f)
                return b;

        const glm::vec3 center { 0.0f, 0.0f, -10.0f };
        const glm::vec3 v1 = glm::normalize(a - center);
        const glm::vec3 v2 = glm::normalize(b - center);

        // Compute the angle between the two vectors
        float alpha = std::acos(glm::clamp(glm::dot(v1, v2), -1.0f, 1.0f));

        // Determine the sign of the angle using the cross product
        glm::vec3 cross = glm::cross(v1, v2);
        if (glm::dot(cross, n) > 0) {
                alpha = -alpha; // Make angle negative for clockwise rotation
        }

        return center + glm::rotate((a - center), alpha * c, n);
}

inline glm::vec3 test(const glm::vec3& a, const glm::vec3& n, float theta, float c)
{
        glm::vec3 center = {0.0f, 0.0f, -10.0f};
        glm::vec3 translated = a - center;

        float angle = theta * c;

        return glm::rotate(translated, angle, n) + center;
}