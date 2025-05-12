#include "CallbacksManager.h"

#include <glad/glad.h>

#include "../OpenGL/Window.h"
#include "InputManager.h"

void CallbacksManager::setCallbacks() {
        GLFWwindow *win = *Window::get();
        glfwSetFramebufferSizeCallback(win, CallbacksManager::framebufferResizeCallback);
        glfwSetKeyCallback(win, CallbacksManager::keyCallback);
        glfwSetMouseButtonCallback(win, CallbacksManager::mouseButtonCallback);
        glfwSetScrollCallback(win, CallbacksManager::mouseScrollCallback);
}

void CallbacksManager::addKeyCallbackLambda(const std::function<void()>& lambda, int* key, int action) {
        keyCallbackLambdas.emplace_back(lambda, key, action);
}

void CallbacksManager::addMouseButtonCallbackLambda(const std::function<void()>& lambda, int* button, int action) {
        mouseButtonCallbackLambdas.emplace_back(lambda, button, action);
}

void CallbacksManager::addMouseScrollCallbackLambda(const std::function<void(glm::vec2)>& lambda) {
        mouseScrollCallbackLambdas.emplace_back(lambda);
}

void CallbacksManager::addViewportResizeCallbackLambda(const std::function<void(int, int)>& lambda) {
        viewportResizeCallbackLambdas.emplace_back(lambda);
}

void CallbacksManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        for (const auto& tuple : keyCallbackLambdas) {
                if (*std::get<1>(tuple) == key && std::get<2>(tuple) == action /*&& !InputManager::imGuiIO->WantCaptureKeyboard*/) std::get<0>(tuple)();
        }
}

void CallbacksManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        for (const auto& tuple : mouseButtonCallbackLambdas) {
                if (*std::get<1>(tuple) == button && std::get<2>(tuple) == action /*&& !InputManager::imGuiIO->WantCaptureMouse*/) std::get<0>(tuple)();
        }
}

void CallbacksManager::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
        for (const auto& lambda : mouseScrollCallbackLambdas) {
                lambda({xOffset, yOffset});
        }
}

void CallbacksManager::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        Window::get()->m_width  = width;
        Window::get()->m_height = height;

        for (const auto& lambda : viewportResizeCallbackLambdas) {
                lambda(width, height);
        }

        glViewport(0, 0, width, height);
}


