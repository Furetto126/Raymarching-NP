#pragma once

#include <glm/glm.hpp>

#include "../Base.h"

$SSBO(Sphere,
        glm::vec3, origin,
        float,     radius,

        glm::vec3, color,
        int,       shoudlRender
);

