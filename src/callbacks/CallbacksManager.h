#pragma once

#include <functional>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

class CallbacksManager {
public:
        static void addKeyCallbackLambda           (const std::function<void()> &lambda, int *key, int action);
        static void addMouseButtonCallbackLambda   (const std::function<void()> &lambda, int *button, int action);
        static void addMouseScrollCallbackLambda   (const std::function<void(glm::vec2)> &lambda);
        static void addViewportResizeCallbackLambda(const std::function<void(int width, int height)>& lambda);

        static void setCallbacks();

private:
        static inline std::vector<std::tuple<std::function<void()>, int *, int>>        keyCallbackLambdas;
        static inline std::vector<std::tuple<std::function<void()>, int *, int>>        mouseButtonCallbackLambdas;
        static inline std::vector<std::function           <void(glm::vec2)>>            mouseScrollCallbackLambdas;
        static inline std::vector<std::function           <void(int width, int height)>>viewportResizeCallbackLambdas;

        static void keyCallback                    (GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseButtonCallback            (GLFWwindow* window, int button, int action, int mods);
        static void mouseScrollCallback            (GLFWwindow* window, double xOffset, double yOffset);
        static void framebufferResizeCallback      (GLFWwindow* window, int width, int height);
};
