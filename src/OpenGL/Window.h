#pragma once

#include <GLFW/glfw3.h>

class Window {
public:
        uint32_t m_width;
        uint32_t m_height;

        static Window *get()
        {
                static Window window(1920, 1080, "Ray marching");
                return &window;
        }

        inline operator GLFWwindow *() const
        {
                return m_pWindow;
        }

private:
        GLFWwindow *m_pWindow;

        Window(uint32_t width, uint32_t height, const char *title);

};