#pragma once

#include <VkBootstrap.h>

#include <vulkan/vulkan.hpp>

#include "aetherion/gpu/backend/driver.hpp"

namespace aetherion {
    class VulkanDriver : public IGPUDriver {
      public:
        VulkanDriver() = delete;
        VulkanDriver(const GPUDriverDescription& description);
        // No need for a direct constructor from vk::Instance as the driver creates and owns it.
        ~VulkanDriver() noexcept override;

        VulkanDriver(const VulkanDriver&) = delete;
        VulkanDriver& operator=(const VulkanDriver&) = delete;

        VulkanDriver(VulkanDriver&&) noexcept;
        VulkanDriver& operator=(VulkanDriver&&) noexcept;

        std::unique_ptr<IGPUPhysicalDevice> createPhysicalDevice(
            const PhysicalGPUDeviceDescription& description) override;

        std::unique_ptr<IGPUDevice> createDevice(
            const GPUDeviceDescription& deviceDescription) override;

        std::unique_ptr<IRenderSurface> createSurface(
            const RenderSurfaceDescription& description) override;

        inline vkb::Instance getVkBuilderInstance() const { return builderInstance_; }
        inline vk::Instance getVkInstance() const { return instance_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vkb::Instance builderInstance_;
        vk::Instance instance_;
    };
}  // namespace aetherion