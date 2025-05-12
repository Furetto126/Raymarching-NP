#include "Window.h"

#include <glad/glad.h>

#include "../Base.h"
#include "../Logger.h"
#include "OpenGLLog.h"

Window::Window(uint32_t width, uint32_t height, const char *title): m_width(width), m_height(height)
{
        if (!glfwInit())
                log::error("Could not initialize GLFW.");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        m_pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_pWindow) {
                glfwTerminate();
                log::error("Failed to create GLFW window.");
        }
        glfwMakeContextCurrent(m_pWindow);
        glfwSwapInterval(0);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
                log::error("Failed to initialize GLAD.");

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
}