#pragma once

#include "aetherion/platform/window_manager.hpp"

namespace aetherion {
    class GLFWWindowManager : public IWindowManager {
      public:
        GLFWWindowManager();
        ~GLFWWindowManager() override;

        GLFWWindowManager(const GLFWWindowManager&) = delete;
        GLFWWindowManager& operator=(const GLFWWindowManager&) = delete;

        GLFWWindowManager(GLFWWindowManager&&) noexcept = default;
        GLFWWindowManager& operator=(GLFWWindowManager&&) noexcept = default;

        virtual void pollEvents() override;

        virtual std::unique_ptr<IWindow> createWindow(
            const WindowDescription& description) override;

        void clear() noexcept;
        void release() noexcept;

      private:
        static void errorCallback(int error, const char* description);
    };
}  // namespace aetherion