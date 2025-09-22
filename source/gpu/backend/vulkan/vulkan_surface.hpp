#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/gpu/backend/surface.hpp"
#include "aetherion/platform/window.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDriver;
    class VulkanImage;

    class VulkanSurface : public IRenderSurface {
      public:
        VulkanSurface() = delete;
        VulkanSurface(VulkanDriver& driver, const RenderSurfaceDescription& description);
        VulkanSurface(vk::Instance instance, vk::SurfaceKHR surface);
        ~VulkanSurface() noexcept override;

        VulkanSurface(const VulkanSurface&) = delete;
        VulkanSurface& operator=(const VulkanSurface&) = delete;

        VulkanSurface(VulkanSurface&&) noexcept;
        VulkanSurface& operator=(VulkanSurface&&) noexcept;

        std::vector<RenderSurfaceFormat> getSupportedFormats(
            const IGPUPhysicalDevice& physicalDevice) const override;

        inline vk::SurfaceKHR getVkSurface() const { return surface_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Instance instance_;

        vk::SurfaceKHR surface_;
    };
}  // namespace aetherion