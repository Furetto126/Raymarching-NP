#pragma once

#include <GLFW/glfw3.h>

struct {
        int   closeWindowButton            = GLFW_KEY_ESCAPE;

        // Camera settings
        float cameraZoomSpeed              = 1.0f;
        float cameraPanSpeed               = 0.01f;
        float cameraRotationSpeed          = 0.01f;
        float cameraFOV                    = 90.0f;
        int   cameraPanButton              = GLFW_MOUSE_BUTTON_MIDDLE;
        int   cameraRotationButton         = GLFW_MOUSE_BUTTON_RIGHT;

        // Scene controls
        int   selectObjectInScene          = GLFW_MOUSE_BUTTON_LEFT;

} engineOptions{};
