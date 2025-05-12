#pragma once

#include <yaml-cpp/yaml.h>

#include "scene/KeyFrame.h"

class YAMLLoader {
public:
        static inline std::vector<KeyFrame> s_keyframes;
        static inline std::unordered_map<std::string, uint32_t> s_times;

        static void load();

};