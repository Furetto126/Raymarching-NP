#include "Camera.h"

#include "../callbacks/CallbacksManager.h"
#include "../callbacks/InputManager.h"
#include "../OpenGL/Window.h"

Camera::Camera(ShaderProgram &shader) : m_shader(&shader)
{
        initCameraControls();
        initUniforms();
}

void Camera::initCameraControls()
{
        // Middle button -> Pan camera
        // ---------------------------
        auto mouse_panCameraPressCallback = [this]() -> void {
                if (InputManager::isMouseButtonDown[engineOptions.cameraPanButton])
                        return;

                //ImGui::SetWindowFocus();
                panCamera(InputManager::getMousePosition());

                glfwSetInputMode(*Window::get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                InputManager::isMouseButtonDown[engineOptions.cameraPanButton] = true;
        };

        auto mouse_panCameraReleaseCallback = []() -> void {
                InputManager::isMouseButtonDown[engineOptions.cameraPanButton] = false;
                glfwSetInputMode(*Window::get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        };

        CallbacksManager::addMouseButtonCallbackLambda(mouse_panCameraPressCallback,   &engineOptions.cameraPanButton, GLFW_PRESS);
        CallbacksManager::addMouseButtonCallbackLambda(mouse_panCameraReleaseCallback, &engineOptions.cameraPanButton, GLFW_RELEASE);

        // Right button -> rotate camera
        // -----------------------------
        auto mouse_cameraRotationButtonPressCallback = [this] {
                if (InputManager::isMouseButtonDown[engineOptions.cameraPanButton]) return;

                // ImGui::SetWindowFocus();
                rotateCamera(InputManager::getMousePosition());

                glfwSetInputMode(*Window::get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                InputManager::isMouseButtonDown[engineOptions.cameraRotationButton] = true;
        };

        auto mouse_cameraRotationButtonReleaseCallback = [] {
                //ImGui::SetWindowFocus();
                InputManager::isMouseButtonDown[engineOptions.cameraRotationButton] = false;
                glfwSetInputMode(*Window::get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        };

        CallbacksManager::addMouseButtonCallbackLambda(mouse_cameraRotationButtonPressCallback,   &engineOptions.cameraRotationButton, GLFW_PRESS);
        CallbacksManager::addMouseButtonCallbackLambda(mouse_cameraRotationButtonReleaseCallback, &engineOptions.cameraRotationButton, GLFW_RELEASE);

        // Scroll wheel -> zoom camera
        // ---------------------------
        auto scroll_cameraZoomCallback = [this](glm::vec2 offset) {
                zoomCamera(offset.y);
        };
        CallbacksManager::addMouseScrollCallbackLambda(scroll_cameraZoomCallback);
}

void Camera::updateCamera()
{
        auto pos = InputManager::getMousePosition();

        if (InputManager::isMouseButtonDown[engineOptions.cameraPanButton])
                panCamera(pos);
        if (InputManager::isMouseButtonDown[engineOptions.cameraRotationButton])
                rotateCamera(pos);
        else if (!InputManager::isMouseButtonDown[engineOptions.cameraPanButton]
                && !InputManager::isMouseButtonDown[engineOptions.cameraRotationButton])
                lastMousePos = pos;

        front = glm::normalize(position - glm::vec3(0.0, 0.0, -10.0));
        right = glm::normalize(glm::cross(front, up));
        inverseViewMatrix = glm::inverse(glm::lookAt(position, position + front, up));

        m_shader->setUniform("FOV", FOV);
        m_shader->setUniform("cameraPosition", position);
        m_shader->setUniform("cameraDirection", front);
        m_shader->setUniform("cameraRight", right);
        m_shader->setUniform("inverseViewMatrix", inverseViewMatrix);
}

void Camera::panCamera(glm::vec2 mousePos)
{
        glm::vec2 cursorOffset = mousePos - lastMousePos;
        lastMousePos = mousePos;

        float xOffset = -cursorOffset.x;
        float yOffset = -cursorOffset.y;

        glm::vec3 tangent = glm::normalize(glm::vec3(front.z, 0.0, -front.x));
        glm::vec3 bitangent = glm::cross(front, tangent);

        glm::vec3 movement = (tangent * xOffset + bitangent * yOffset) * engineOptions.cameraPanSpeed;
        position += movement;
}

void Camera::zoomCamera(double yOffset)
{
        position += front * -(float)yOffset * engineOptions.cameraZoomSpeed;
        updateCamera();
}

void Camera::rotateCamera(glm::vec2 mousePos)
{
        glm::vec2 cursorOffset = mousePos - lastMousePos;
        lastMousePos = mousePos;

        float xOffset = -cursorOffset.x;
        float yOffset = -cursorOffset.y;

        front += (right * xOffset - up * yOffset) * engineOptions.cameraRotationSpeed;
        front = glm::normalize(front);
}

void Camera::initUniforms()
{
        m_shader->addUniform("FOV", FOV);
        m_shader->addUniform("cameraPosition", position);
        m_shader->addUniform("cameraDirection", front);
        m_shader->addUniform("cameraRight", right);
        m_shader->addUniform("inverseViewMatrix", inverseViewMatrix);
}