#pragma once

#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/util/common_definitions.hpp"
#include "aetherion/util/result_value.hpp"

namespace aetherion {
    // Forward declarations
    class IImage;
    class IImageView;
    class IGPUBinarySemaphore;
    class IGPUFence;
    class ISurface;

    struct SwapchainDescription {
        const ISurface* surface;
        uint32_t minImageCount = 3;
        SurfaceFormat surfaceFormat;
        Extent2Du extent;
        uint32_t arrayLayers = 1;
        ImageUsageFlags imageUsages = ImageUsage::None;
        SharingMode sharingMode;
        std::vector<uint32_t> queueFamilyIndices;
        PresentMode presentMode;
        bool clipped = false;
    };

    class ISwapchain {
      public:
        virtual ~ISwapchain() = 0;

        ISwapchain(const ISwapchain&) = delete;
        ISwapchain& operator=(const ISwapchain&) = delete;

        virtual ResultValue<SwapchainAcquireResultCode, uint32_t> acquireNextImage(
            uint64_t timeout, IGPUBinarySemaphore& semaphore, IGPUFence& fence)
            = 0;

        virtual uint32_t getImageCount() const = 0;

        virtual IImage& getImage(uint32_t index) = 0;

      protected:
        ISwapchain() = default;
        ISwapchain(ISwapchain&&) noexcept = default;
        ISwapchain& operator=(ISwapchain&&) noexcept = default;
    };
}  // namespace aetherion