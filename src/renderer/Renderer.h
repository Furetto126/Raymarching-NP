#pragma once

#include <string>
#include <thread>
#include <glad/glad.h>

#include "../OpenGL/Window.h"

namespace Renderer {
        const std::string outputDir = "../../../resources/render/";

        void saveFramesWorker(const std::string &outputFileName, GLsizei bufferSize);
        void saveFramebuffer(Window& window);
        void cleanupWorkerThread(std::thread &workerThread);
};
