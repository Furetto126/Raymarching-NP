#include "InputManager.h"

#include <GLFW/glfw3.h>

#include "../OpenGL/Window.h"

bool InputManager::getKeyPressed(int key)
{
        return glfwGetKey(*Window::get(), key) == GLFW_PRESS;
}

bool InputManager::getKeyPressed(int key, int mods)
{
        return glfwGetKey(*Window::get(), key) == GLFW_PRESS && glfwGetKey(*Window::get(), mods) == GLFW_PRESS;
}

glm::vec2 InputManager::getMousePosition()
{
        double x = 0.0, y = 0.0;
        glfwGetCursorPos(*Window::get(), &x, &y);
        return {x, y};
}
