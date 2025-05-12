#include "SphereManager.h"

#include <GLFW/glfw3.h>

#include "../MergeFunctions.h"

void SphereManager::update(Camera& camera)
{
        // The index should start at 1, since below we access i - 1, but due to
        // how the for modified works, the increment is done even when the condition
        // is false, so we start it one value above what it should be, then we decrease
        // it by one.
        uint32_t i = 1;
        for (; (i < YAMLLoader::s_keyframes.size()); ++i)
                if (YAMLLoader::s_keyframes[i].m_start >= s_frames)
                        break;

        i = std::min(i, (uint32_t)YAMLLoader::s_keyframes.size() - 1);

        static uint32_t currentKeyframe = 0;
        if (currentKeyframe != i - 1) {
            currentKeyframe = i - 1;
            std::cout << "Current keyframe: " << currentKeyframe << " [" << YAMLLoader::s_keyframes[i].m_start << "]\n";
        }

        const KeyFrame &currentKeyFrame = YAMLLoader::s_keyframes[i - 1];
        const KeyFrame &nextKeyFrame    = YAMLLoader::s_keyframes[i];

        auto kf = KeyFrame::merge(currentKeyFrame, nextKeyFrame, s_frames);
        for (const auto &[idx, pos] : kf.m_pos)
                if (idx < s_spheres.size())
                        s_spheres[idx].origin = pos;
        for (const auto &[idx, col] : kf.m_colors)
                if (idx < s_spheres.size())
                        s_spheres[idx].color = col;
        for (const auto &[idx, rad] : kf.m_radii)
                if (idx < s_spheres.size())
                        s_spheres[idx].radius = rad;
        for (const auto &[idx, render] : kf.m_shouldRender)
                if (idx < s_spheres.size())
                        s_spheres[idx].shoudlRender = render;

        camera.position = kf.m_cameraPos;
}