#pragma once

#include <VkBootstrap.h>

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/driver.hpp"

namespace aetherion {
    class VulkanDriver : public IRenderDriver {
      public:
        VulkanDriver() = delete;
        VulkanDriver(const RenderDriverDescription& description);
        // No need for a direct constructor from vk::Instance as the driver creates and owns it.
        ~VulkanDriver() noexcept override;

        VulkanDriver(const VulkanDriver&) = delete;
        VulkanDriver& operator=(const VulkanDriver&) = delete;

        VulkanDriver(VulkanDriver&&) noexcept;
        VulkanDriver& operator=(VulkanDriver&&) noexcept;

        virtual std::unique_ptr<IPhysicalDevice> createPhysicalDevice(
            const PhysicalDeviceDescription& description) override;

        virtual std::unique_ptr<IRenderDevice> createDevice(
            const DeviceDescription& deviceDescription) override;

        virtual std::unique_ptr<ISurface> createSurface(
            const SurfaceDescription& description) override;

        inline vkb::Instance getVkBuilderInstance() const { return builderInstance_; }
        inline vk::Instance getVkInstance() const { return instance_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vkb::Instance builderInstance_;
        vk::Instance instance_;
    };
}  // namespace aetherion