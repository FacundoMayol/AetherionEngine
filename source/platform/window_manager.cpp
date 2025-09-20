#include "aetherion/platform/window_manager.hpp"

#include "glfw/glfw_window_manager.hpp"

namespace aetherion {
    std::unique_ptr<IWindowManager> IWindowManager::create(WindowManagerType type) {
        switch (type) {
            case WindowManagerType::GLFW:
                return std::make_unique<GLFWWindowManager>();
            default:
                throw std::invalid_argument("Unsupported window manager type.");
        }
    }

    IWindowManager::~IWindowManager() = default;
}  // namespace aetherion