#include "YAMLConverter.h"

#include <iostream>
#include <random>
#include <chrono>
#include <ranges>

#include "MergeFunctions.h"
#include "GLFW/glfw3.h"
#include "Logger.h"

template<>
struct YAML::convert<glm::vec3> {
        static Node encode(const glm::vec3 &rhs) {
                Node node;
                node.push_back(rhs.x);
                node.push_back(rhs.y);
                node.push_back(rhs.z);
                return node;
        }

        static bool decode(const Node &node, glm::vec3 &rhs) {
                if(!node.IsSequence() || node.size() != 3)
                        return false;

                rhs.x = node[0].as<glm::vec3::value_type>();
                rhs.y = node[1].as<glm::vec3::value_type>();
                rhs.z = node[2].as<glm::vec3::value_type>();
                return true;
        }
};

template<typename K, typename V>
struct YAML::convert<std::unordered_map<K, V>> {
        static Node encode(const std::unordered_map<K, V> &rhs) {
                Node node;
                for (const auto &[k, v] : rhs)
                        node[k] = v;

                return node;
        }

        static bool decode(const Node &node, std::unordered_map<K, V> &rhs) {
                if(!node.IsMap())
                        return false;

                for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
                        rhs.insert({ it->first.as<K>(), it->second.as<V>() });

                return true;
        }
};

static inline decltype(KeyFrame::m_mergePositions) getFunction(const std::string &id) {
        if (id == "glm::mix")
                return glm::mix;
        if (id == "easeIn")
                return easeIn;
        if (id == "easeOut")
                return easeOut;

        return glm::mix;
}

// I shall ask god for forgiveness after this

static decltype(KeyFrame::m_radii) randomRadii() {
        std::default_random_engine gen((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> dis(0.6f, 1.4f);

        decltype(KeyFrame::m_radii) radii = {
                { 0, dis(gen) },
                { 1, dis(gen) },
                { 2, dis(gen) },
                { 3, dis(gen) },
                { 4, dis(gen) },
                { 5, dis(gen) },
                { 6, dis(gen) },
                { 7, dis(gen) },
                { 8, dis(gen) },
                { 9, dis(gen) }
        };

        return radii;
}

// Render 1 sphere
static decltype(KeyFrame::m_shouldRender) randomRender1() {
        std::default_random_engine gen((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution dis(0, 9);
        int v = dis(gen);

        decltype(KeyFrame::m_shouldRender) render = {
                { 0, v == 0 },
                { 1, v == 1 },
                { 2, v == 2 },
                { 3, v == 3 },
                { 4, v == 4 },
                { 5, v == 5 },
                { 6, v == 6 },
                { 7, v == 7 },
                { 8, v == 8 },
                { 9, v == 9 }
        };

        return render;
}

// Render all rows but 1
static decltype(KeyFrame::m_shouldRender) randomRender2() {
        std::default_random_engine gen((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution dis(0, 3);
        int v = dis(gen);

        decltype(KeyFrame::m_shouldRender) render = {
                { 0, v != 0 },
                { 1, v != 1 },
                { 2, v != 1 },
                { 3, v != 2 },
                { 4, v != 2 },
                { 5, v != 2 },
                { 6, v != 3 },
                { 7, v != 3 },
                { 8, v != 3 },
                { 9, v != 3 }
        };

        return render;
}

// Render 2 spheres
static decltype(KeyFrame::m_shouldRender) randomRender3() {
        std::default_random_engine gen((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution dis(0, 9);
        int v1 = dis(gen);
        int v2 = dis(gen);

        decltype(KeyFrame::m_shouldRender) render = {
                { 0, v1 == 0 || v2 == 0 },
                { 1, v1 == 1 || v2 == 1 },
                { 2, v1 == 2 || v2 == 2 },
                { 3, v1 == 3 || v2 == 3 },
                { 4, v1 == 4 || v2 == 4 },
                { 5, v1 == 5 || v2 == 5 },
                { 6, v1 == 6 || v2 == 6 },
                { 7, v1 == 7 || v2 == 7 },
                { 8, v1 == 8 || v2 == 8 },
                { 9, v1 == 9 || v2 == 9 }
        };

        return render;
}

void YAMLLoader::load()
{
        YAML::Node config{};
        try {
                config = YAML::LoadFile("../../../resources/keyframes.yaml");
        } catch (const std::exception &e) {
                // MSVC
                std::cout << e.what() << std::endl;
                throw;
        }

        const auto start = config["start"].as<uint32_t>();

        // Get times
        for (YAML::const_iterator it = config.begin(); it != config.end(); ++it) {
                if (!it->first.as<std::string>().starts_with("t_"))
                        continue;
                
                s_times.insert({ it->first.as<std::string>(), it->second.as<uint32_t>() + start });
        }

        // Get keyframes
        for (auto kf : config["keyframes"]) {
                KeyFrame keyFrame{};
                if (!s_keyframes.empty())
                        keyFrame = s_keyframes.back();

                if (kf["positions"])
                        keyFrame.m_pos = kf["positions"].as<std::unordered_map<uint32_t, glm::vec3>>();

                if (kf["colors"])
                        keyFrame.m_colors = kf["colors"].as<std::unordered_map<uint32_t, glm::vec3>>();

                if (kf["rotationNormals"])
                        keyFrame.m_rotationNormals = kf["rotationNormals"].as<std::unordered_map<uint32_t, glm::vec3>>();

                if (kf["radii"])
                        keyFrame.m_radii = kf["radii"].as<std::unordered_map<uint32_t, float>>();

                if (kf["render"])
                        keyFrame.m_shouldRender = kf["render"].as<std::unordered_map<uint32_t, int>>();

                if (kf["camera"])
                        keyFrame.m_cameraPos = kf["camera"].as<glm::vec3>();

                if (kf["cameraNormal"])
                        keyFrame.m_cameraRotationNormal = kf["cameraNormal"].as<glm::vec3>();

                if (kf["start"])
                        keyFrame.m_start = kf["start"].as<uint32_t>() + start;
                else
                        log::error("Keyframe start property requires.");

                if (kf["angles"])
                        keyFrame.m_angles = kf["angles"].as<std::unordered_map<uint32_t, float>>();

                if (kf["merge"]) {
                        if (kf["merge"]["pos"]) {
                                const auto name = kf["merge"]["pos"].as<std::string>();
                                if (name != "orbit") {
                                        keyFrame.m_mergePositions = getFunction(name);
                                        keyFrame.m_mergePositionsOrbit = nullptr;
                                } else {
                                        keyFrame.m_mergePositionsOrbit = test;
                                }
                        }

                        if (kf["merge"]["col"])
                                keyFrame.m_mergeColors = getFunction(kf["merge"]["col"].as<std::string>());
                }

                s_keyframes.push_back(keyFrame);
        }

        // TODO: IF THINGS BREAK DOWN CHECK HERE!!
        const auto &trianglePos = s_keyframes[6].m_pos;
        const auto &finalColors = s_keyframes[9].m_colors;
        const auto &radii = s_keyframes[9].m_radii;

        const uint32_t randomStart = s_times.at("t_rn0");
        const uint32_t end = s_times.at("t_rn1");
        const uint32_t dur = (end - randomStart) / 3;
        const uint32_t rndFrame = dur / 10;

        std::vector<KeyFrame> randomPhase;
        randomPhase.reserve(30);

        // 1200 - 1740
        for (uint32_t i{}; i < 10; ++i) {
                randomPhase.push_back({
                        .m_pos = trianglePos,
                        .m_colors = finalColors,
                        .m_radii = radii,
                        .m_shouldRender = randomRender1(),
                        .m_cameraPos = { 0.0f, 0.0f, 0.0f },
                        .m_cameraRotationNormal = { 0.0f, 1.0f, 0.0f },
                        .m_start = randomStart + i * rndFrame,
                        .m_mergeRadii = easeIn,
                        .m_mergePositions = glm::mix,
                        .m_mergeColors = glm::mix
                });
        }

        // 1800 - 2340
        for (uint32_t i{}; i < 10; ++i) {
                randomPhase.push_back({
                        .m_pos = trianglePos,
                        .m_colors = finalColors,
                        .m_radii = i ? randomRadii() : radii,
                        .m_shouldRender = randomRender2(),
                        .m_cameraPos = { 0.0f, 0.0f, 0.0f },
                        .m_cameraRotationNormal = { 0.0f, 1.0f, 0.0f },
                        .m_start = (randomStart + dur) + i * rndFrame,
                        .m_mergeRadii = easeIn,
                        .m_mergePositions = glm::mix,
                        .m_mergeColors = glm::mix
                });
        }

        // 2400 - 2940
        for (uint32_t i{}; i < 10; ++i) {
                randomPhase.push_back({
                        .m_pos = trianglePos,
                        .m_colors = finalColors,
                        .m_radii = radii,
                        .m_shouldRender = randomRender3(),
                        .m_cameraPos = { 0.0f, 0.0f, 0.0f },
                        .m_cameraRotationNormal = { 0.0f, 1.0f, 0.0f },
                        .m_start = (randomStart + dur * 2) + i * rndFrame,
                        .m_mergeRadii = easeIn,
                        .m_mergePositions = glm::mix,
                        .m_mergeColors = glm::mix
                });
        }

        // Find the position of the random phase end
        auto it = std::ranges::find_if(s_keyframes, [](const KeyFrame &kf) -> bool {
                return kf.m_start == s_times["t_rn1"];
        });

        // Add random phase to the keyframes
        s_keyframes.insert_range(it, randomPhase);
}