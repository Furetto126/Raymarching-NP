#include "Renderer.h"

#include <condition_variable>
#include <fstream>
#include <queue>
#include <mutex>

#include "../Logger.h"

// Absolutely stolen from chat gpt
static std::queue<std::vector<GLbyte>> frameQueue;
static std::mutex queueMutex;
static std::condition_variable frameAvailable;
static bool stopSaving = false;

void Renderer::saveFramesWorker(const std::string &outputFileName, GLsizei bufferSize) {
        std::ofstream file(outputFileName, std::ios::binary);

        if (!file.is_open())
                log::error("Failed to open output file for raw frames.");

        while (true) {
                std::vector<GLbyte> frameData;
                {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        frameAvailable.wait(lock, [] { return !frameQueue.empty() || stopSaving; });

                        if (stopSaving && frameQueue.empty()) break;

                        frameData = std::move(frameQueue.front());
                        frameQueue.pop();
                }

                // Write raw frame data to the file
                file.write(reinterpret_cast<const char*>(frameData.data()), bufferSize);
        }

        file.close();
}

void Renderer::saveFramebuffer(Window& window)
{
        const GLsizei nrChannels = 4;
        const GLsizei stride = nrChannels * window.m_width;
        const GLsizei bufferSize = stride * window.m_height;

        std::vector<GLbyte> buffer(bufferSize);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadBuffer(GL_FRONT);
        glReadPixels(0, 0, window.m_width, window.m_height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

        // Push the raw frame data into the queue for the worker thread to handle
        {
                std::lock_guard<std::mutex> lock(queueMutex);
                frameQueue.push(std::move(buffer));
        }
        frameAvailable.notify_one();
}

void Renderer::cleanupWorkerThread(std::thread &workerThread) {
        {
                std::lock_guard<std::mutex> lock(queueMutex);
                stopSaving = true;
        }
        frameAvailable.notify_all();
        workerThread.join();
}
