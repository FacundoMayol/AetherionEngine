#pragma once

#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/platform/window.hpp"

namespace aetherion {
    // Forward declarations
    class IPhysicalDevice;
    class IWindow;

    struct SurfaceDescription {
        IWindow* window;
    };

    class ISurface {
      public:
        virtual ~ISurface() = 0;

        ISurface(const ISurface&) = delete;
        ISurface& operator=(const ISurface&) = delete;

        virtual std::vector<SurfaceFormat> getSupportedFormats(
            const IPhysicalDevice& physicalDevice) const
            = 0;

      protected:
        ISurface() = default;
        ISurface(ISurface&&) noexcept = default;
        ISurface& operator=(ISurface&&) noexcept = default;
    };
}  // namespace aetherion
