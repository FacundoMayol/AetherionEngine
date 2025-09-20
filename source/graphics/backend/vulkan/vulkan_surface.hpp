#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/surface.hpp"
#include "aetherion/platform/window.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDriver;
    class VulkanImage;

    class VulkanSurface : public ISurface {
      public:
        VulkanSurface() = delete;
        VulkanSurface(VulkanDriver& driver, const SurfaceDescription& description);
        VulkanSurface(VulkanDriver& driver, vk::SurfaceKHR surface);
        virtual ~VulkanSurface() noexcept override;

        VulkanSurface(const VulkanSurface&) = delete;
        VulkanSurface& operator=(const VulkanSurface&) = delete;

        VulkanSurface(VulkanSurface&&) noexcept;
        VulkanSurface& operator=(VulkanSurface&&) noexcept;

        virtual std::vector<SurfaceFormat> getSupportedFormats(
            const IPhysicalDevice& physicalDevice) const override;

        inline vk::SurfaceKHR getVkSurface() const { return surface_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDriver* driver_;

        vk::SurfaceKHR surface_;
    };
}  // namespace aetherion