#include "glfw_window.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vulkan/vulkan.hpp>

namespace aetherion {
    GLFWWindow::GLFWWindow(GLFWWindowManager& manager, const WindowDescription& description)
        : manager_(&manager) {
        window_ = glfwCreateWindow(description.extent.width, description.extent.height,
                                   description.title.c_str(), nullptr, nullptr);
        if (!window_) {
            throw std::runtime_error("Failed to create GLFW window");
        }
    }

    GLFWWindow::GLFWWindow(GLFWWindowManager& manager, GLFWwindow* window)
        : manager_(&manager), window_(window) {
        if (!window_) {
            throw std::invalid_argument("GLFW window handle cannot be null.");
        }
    }

    GLFWWindow::~GLFWWindow() { clear(); }

    GLFWWindow::GLFWWindow(GLFWWindow&& other) noexcept
        : IWindow(std::move(other)), manager_(other.manager_), window_(other.window_) {
        other.manager_ = nullptr;
        other.window_ = nullptr;
    }

    GLFWWindow& GLFWWindow::operator=(GLFWWindow&& other) noexcept {
        if (this != &other) {
            clear();

            IWindow::operator=(std::move(other));
            manager_ = other.manager_;
            window_ = other.window_;

            other.release();
        }
        return *this;
    }

    void GLFWWindow::clear() noexcept {
        if (window_) {
            glfwDestroyWindow(window_);
            window_ = nullptr;
        }
        manager_ = nullptr;
    }

    void GLFWWindow::release() noexcept {
        window_ = nullptr;
        manager_ = nullptr;
    }

    vk::SurfaceKHR GLFWWindow::createVulkanSurface(vk::Instance instance) const {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), window_, nullptr, &surface)
            != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan surface for GLFW window.");
        }
        return vk::SurfaceKHR(surface);
    }

    bool GLFWWindow::shouldClose() const { return glfwWindowShouldClose(window_); }

    void GLFWWindow::setShouldClose(bool value) { glfwSetWindowShouldClose(window_, value); }
}  // namespace aetherion