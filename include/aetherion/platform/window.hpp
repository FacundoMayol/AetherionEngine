#pragma once

#include <string>

#include "aetherion/util/common_definitions.hpp"

// Forward declarations
namespace vk {
    class Instance;
    class SurfaceKHR;
    class Extent2D;
}  // namespace vk

namespace aetherion {
    using WindowExtent = Extent2Du;

    struct WindowDescription {
        std::string title;
        WindowExtent extent;
    };

    class IWindow {
      public:
        virtual ~IWindow() = 0;

        IWindow(const IWindow&) = delete;
        IWindow& operator=(const IWindow&) = delete;

        virtual vk::SurfaceKHR createVulkanSurface(vk::Instance instance) const = 0;

        virtual bool shouldClose() const = 0;

        virtual void setShouldClose(bool value) = 0;

      protected:
        IWindow() = default;
        IWindow(IWindow&&) noexcept = default;
        IWindow& operator=(IWindow&&) noexcept = default;
    };
}  // namespace aetherion