#pragma once

#include <glm/common.hpp>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../Base.h"

struct KeyFrame {
        std::unordered_map<uint32_t, glm::vec3> m_pos;
        std::unordered_map<uint32_t, glm::vec3> m_colors;
        std::unordered_map<uint32_t, glm::vec3> m_rotationNormals;
        std::unordered_map<uint32_t, glm::vec3> m_sphereOrbitNormals;
        std::unordered_map<uint32_t, float>     m_radii;
        std::unordered_map<uint32_t, float>     m_angles;
        std::unordered_map<uint32_t, int>       m_shouldRender;

        glm::vec3 m_cameraPos;
        glm::vec3 m_cameraRotationNormal;

        uint32_t m_start;

        Function<float, float, float, float> m_mergeRadii = glm::mix;
        Function<glm::vec3, const glm::vec3 &, const glm::vec3 &, float> m_mergePositions = glm::mix;
        Function<glm::vec3, const glm::vec3 &, const glm::vec3 &, float, float> m_mergePositionsOrbit = nullptr;
        Function<glm::vec3, const glm::vec3 &, const glm::vec3 &, float> m_mergeColors = [](const glm::vec3& a, const glm::vec3& b, float c) { return a; };

        static KeyFrame merge(const KeyFrame &previous, const KeyFrame &next, uint32_t frame);
};