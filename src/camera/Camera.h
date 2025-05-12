#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../shaders/Shader.h"
#include "../EngineOptions.h"

class Camera {
public:
        ShaderProgram *const m_shader;
        glm::mat4 inverseViewMatrix = glm::mat4();

        explicit Camera(ShaderProgram &shader);

        glm::vec3 position = glm::vec3(0.0f);

        void updateCamera();
        [[nodiscard]] glm::vec3 getPosition() const { return position; }

private:
        float &FOV = engineOptions.cameraFOV;

        glm::vec2 lastMousePos = glm::vec2(0.0f);
        glm::vec3 front = { 0.0f, 0.0f, 1.0f };
        glm::vec3 up = { 0.0f, 1.0f, 0.0f };
        glm::vec3 right = { 1.0f, 0.0f, 0.0f };

        void initCameraControls();
        void initUniforms();

        void panCamera(glm::vec2 mousePos);
        void zoomCamera(double yOffset);
        void rotateCamera(glm::vec2 mousePos);
};
