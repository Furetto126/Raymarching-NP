#pragma once

#include <vector>

#include "KeyFrame.h"
#include "Sphere.h"
#include "../camera/Camera.h"
#include "../YAMLConverter.h"

class SphereManager {
public:
        static inline std::vector<Sphere> s_spheres { 10,
                {
                        {0.0f, 0.0f, -10.0f}, 1.0f,
                        glm::vec3(1.0f), false
                }
        };
        static inline uint32_t s_frames = 0;

        static void update(Camera& camera);
    
};
