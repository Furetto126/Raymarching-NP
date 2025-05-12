#include "KeyFrame.h"

#include <algorithm>

#include "../MergeFunctions.h"

static inline glm::vec3 boobas(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &n, float c)
{
        if (a == b)
                return a;

        glm::vec3 center = (a + b) * 0.5f;
        float radius     = glm::distance(a, b) * 0.5f;

        glm::vec3 u = glm::normalize(a - center);
        glm::vec3 v = glm::normalize(glm::cross(n, u));

        float angle = 3.1415926535897932384626433832795028841971f * pow(c, 1.0f + fakeGaussian(c));

        glm::vec3 interpolatedCoords = center + radius * (u * glm::cos(angle) + v * glm::sin(angle));

        return interpolatedCoords;
}

KeyFrame KeyFrame::merge(const KeyFrame &previous, const KeyFrame &next, uint32_t frame)
{
        const auto num = (float)frame - previous.m_start;
        const auto den = (float)next.m_start - previous.m_start;
        float progression = std::clamp(num / den, 0.0f, 1.0f);

        std::unordered_map base_positions = previous.m_pos;
        for (const auto &[id, pos] : next.m_pos) {
                if (base_positions.contains(id)) {
                        if (!previous.m_mergePositionsOrbit)
                                base_positions[id] = previous.m_mergePositions(base_positions[id], pos, progression);
                        else
                                base_positions[id] = previous.m_mergePositionsOrbit(
                                        base_positions[id], previous.m_rotationNormals.at(id),
                                        previous.m_angles.at(id), progression);
                }
        }

        std::unordered_map base_colors = previous.m_colors;
        for (const auto &[id, col] : next.m_colors) {
                if (base_colors.contains(id))
                        base_colors[id] = previous.m_mergeColors(base_colors[id], col, progression);
        }

        std::unordered_map base_radii = previous.m_radii;
        for (const auto &[id, rad] : next.m_radii) {
                if (base_radii.contains(id))
                        base_radii[id] = previous.m_mergeRadii(base_radii[id], rad, progression);
        }

        return {
                .m_pos = base_positions,
                .m_colors = base_colors,
                .m_radii = base_radii,
                .m_shouldRender = previous.m_shouldRender,
                .m_cameraPos = boobas(previous.m_cameraPos, next.m_cameraPos, previous.m_cameraRotationNormal, progression),
                .m_start = next.m_start,
                .m_mergePositions = next.m_mergePositions,
                .m_mergeColors = next.m_mergeColors
        };
}
