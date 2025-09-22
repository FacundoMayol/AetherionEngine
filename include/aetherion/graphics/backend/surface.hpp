#pragma once

#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/platform/window.hpp"

namespace aetherion {
    // Forward declarations
    class IPhysicalDevice;
    class IWindow;

    struct RenderSurfaceDescription {
        IWindow* window;
    };

    class IRenderSurface {
      public:
        virtual ~IRenderSurface() = 0;

        IRenderSurface(const IRenderSurface&) = delete;
        IRenderSurface& operator=(const IRenderSurface&) = delete;

        virtual std::vector<RenderSurfaceFormat> getSupportedFormats(
            const IPhysicalDevice& physicalDevice) const
            = 0;

      protected:
        IRenderSurface() = default;
        IRenderSurface(IRenderSurface&&) noexcept = default;
        IRenderSurface& operator=(IRenderSurface&&) noexcept = default;
    };
}  // namespace aetherion
