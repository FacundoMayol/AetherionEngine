#pragma once

#include <memory>

namespace aetherion {
    // Forward declarations
    class IWindow;
    struct WindowDescription;

    enum WindowManagerType { GLFW, SDL2, Native };

    struct WindowManagerDescription {
        WindowManagerType type;
    };

    class IWindowManager {
      public:
        static std::unique_ptr<IWindowManager> create(WindowManagerType type);

        virtual ~IWindowManager() = 0;

        IWindowManager(const IWindowManager&) = delete;
        IWindowManager& operator=(const IWindowManager&) = delete;

        virtual void pollEvents() = 0;

        virtual std::unique_ptr<IWindow> createWindow(const WindowDescription& description) = 0;

      protected:
        IWindowManager() = default;
        IWindowManager(IWindowManager&&) noexcept = default;
        IWindowManager& operator=(IWindowManager&&) noexcept = default;
    };
}  // namespace aetherion