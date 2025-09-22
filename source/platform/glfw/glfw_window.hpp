#pragma once

#include "aetherion/platform/window.hpp"

// Forward declarations
struct GLFWwindow;

namespace aetherion {
    // Forward declarations
    class GLFWWindowManager;

    class GLFWWindow : public IWindow {
      public:
        GLFWWindow() = delete;
        GLFWWindow(GLFWWindowManager& manager, const WindowDescription& description);
        GLFWWindow(GLFWWindowManager& manager, GLFWwindow* window);
        ~GLFWWindow() override;

        GLFWWindow(const GLFWWindow&) = delete;
        GLFWWindow& operator=(const GLFWWindow&) = delete;

        GLFWWindow(GLFWWindow&&) noexcept;
        GLFWWindow& operator=(GLFWWindow&&) noexcept;

        vk::SurfaceKHR createVulkanSurface(vk::Instance instance) const override;

        inline GLFWwindow* getGlfwWindow() { return window_; }

        bool shouldClose() const override;

        void setShouldClose(bool value) override;

        void clear() noexcept;
        void release() noexcept;

      private:
        GLFWWindowManager* manager_;

        GLFWwindow* window_;
    };
}  // namespace aetherion