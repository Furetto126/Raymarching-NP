#include <iostream>
#include <thread>

#include "OpenGL/Window.h"
#include "shaders/Shader.h"
#include "camera/Camera.h"
#include "shaders/ShadersContent.h"
#include "callbacks/CallbacksManager.h"
#include "scene/Sphere.h"
#include "scene/SphereManager.h"
#include "scene/Cubemap.h"
#include "renderer/Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

void gameLoop()
{
        SphereManager::s_frames++;
        static uint32_t fps{};
        static double lastSecondTime{};

        fps++;

        const double currentTime { glfwGetTime() };
        if (currentTime - lastSecondTime < 1.0)
                return;

        std::cout << "FPS: " << fps << ", frames: " << SphereManager::s_frames << "\n";

        lastSecondTime = currentTime;
        fps = 0;
}

int main()
{
        YAMLLoader::load();

        Window *window = Window::get();
        glfwSwapInterval(1);

        CallbacksManager::setCallbacks();

        ShaderProgram program(VERTEXSHADER, FRAGMENTSHADER);

        SSBO<Sphere> sphereBuffer(0, &SphereManager::s_spheres);
        program.addSSBO(sphereBuffer);

        program.addUniform("time", 0.0f);
        program.addUniform("frames", 0.0f);
        program.addUniform("resolution", glm::vec2(window->m_width, window->m_height));

        // Animation uniforms (to change if animation changes):
        program.addUniform("startRandomPhase", (float)YAMLLoader::s_times.at("t_rn0"));
        program.addUniform("endRandomPhase",   (float)YAMLLoader::s_times.at("t_rn1"));
        program.addUniform("startOrbitPhase",  (float)YAMLLoader::s_times.at("t_or0"));
        program.addUniform("endOrbitPhase",    (float)YAMLLoader::s_times.at("t_or4"));

        Camera camera(program);

        const std::string resourcesDir = "../../../resources/skybox/";
        [[maybe_unused]] GLuint bob = Cubemap::create(
                {
                        resourcesDir + "px.png", // right
                        resourcesDir + "nx.png", // left
                        resourcesDir + "py.png", // top
                        resourcesDir + "ny.png", // bottom
                        resourcesDir + "pz.png", // front
                        resourcesDir + "nz.png"  // back
                }
        );

        std::thread workerThread(Renderer::saveFramesWorker, Renderer::outputDir + "frames.raw", window->m_width * window->m_height * 4);
        while (!glfwWindowShouldClose(*window)) {
                gameLoop();

                SphereManager::update(camera);
                program.setSSBO(sphereBuffer);

                program.draw();

                camera.updateCamera();
                program.setUniform("time", (float)glfwGetTime());
                program.setUniform("frames", (float)SphereManager::s_frames);

                glfwPollEvents();
                glfwSwapBuffers(*window);

                Renderer::saveFramebuffer(*window);

                if (SphereManager::s_frames >= YAMLLoader::s_times.at("t_end") + 240)
                       break;
        }

        Renderer::cleanupWorkerThread(workerThread);
        return 0;
}


