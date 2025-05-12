#pragma once

#include <unordered_map>
#include <glm/vec2.hpp>

class InputManager {
public:
        static inline std::unordered_map<int, bool> isMouseButtonDown;

        static bool getKeyPressed(int key);
        static bool getKeyPressed(int key, int mods);

        static glm::vec2 getMousePosition();
};

