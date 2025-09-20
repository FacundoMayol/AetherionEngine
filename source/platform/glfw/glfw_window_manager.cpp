#include "glfw_window_manager.hpp"

#include <GLFW/glfw3.h>
#include <fmt/format.h>

#include <stdexcept>

#include "glfw_window.hpp"

namespace aetherion {
    GLFWWindowManager::GLFWWindowManager() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwSetErrorCallback(errorCallback);
    }

    GLFWWindowManager::~GLFWWindowManager() { glfwTerminate(); }

    void GLFWWindowManager::clear() noexcept {}

    void GLFWWindowManager::release() noexcept {}

    void GLFWWindowManager::pollEvents() { glfwPollEvents(); }

    std::unique_ptr<IWindow> GLFWWindowManager::createWindow(const WindowDescription& description) {
        return std::make_unique<GLFWWindow>(*this, description);
    }

    void GLFWWindowManager::errorCallback(int error, const char* description) {
        fmt::print(stderr, "GLFW Error ({}): {}\n", error, description);
    }
}  // namespace aetherion