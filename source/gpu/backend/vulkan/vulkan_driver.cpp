#include "vulkan_driver.hpp"

#include <fmt/core.h>

#include "vulkan_device.hpp"
#include "vulkan_surface.hpp"

namespace aetherion {
    VulkanDriver::VulkanDriver(const GPUDriverDescription& description) : IGPUDriver() {
        const auto& vulkanBuilderInstanceResult
            = vkb::InstanceBuilder()
                  .set_app_name(description.name.c_str())
                  .set_engine_name("Aetherion engine")
                  .request_validation_layers(description.validationLayersEnabled)
                  .use_default_debug_messenger()
                  .require_api_version(vk::ApiVersion13)
                  .build();

        if (!vulkanBuilderInstanceResult) {
            throw(std::runtime_error(fmt::format("Failed to create Vulkan instance. Error: {}",
                                                 vulkanBuilderInstanceResult.error().message())));
        }

        builderInstance_ = vulkanBuilderInstanceResult.value();
        instance_ = vk::Instance(builderInstance_.instance);
    }

    VulkanDriver::~VulkanDriver() noexcept { clear(); }

    VulkanDriver::VulkanDriver(VulkanDriver&& other) noexcept
        : IGPUDriver(std::move(other)),
          builderInstance_(std::move(other.builderInstance_)),
          instance_(other.instance_) {
        other.instance_ = nullptr;
    }

    VulkanDriver& VulkanDriver::operator=(VulkanDriver&& other) noexcept {
        if (this != &other) {
            clear();

            IGPUDriver::operator=(std::move(other));
            builderInstance_ = std::move(other.builderInstance_);
            instance_ = other.instance_;

            other.release();
        }
        return *this;
    }

    void VulkanDriver::clear() noexcept {
        if (instance_) {
            instance_.destroy();
            vkb::destroy_instance(builderInstance_);
            instance_ = nullptr;
            builderInstance_ = {};
        }
    }

    void VulkanDriver::release() noexcept {
        instance_ = nullptr;
        builderInstance_ = {};
    }

    std::unique_ptr<IGPUPhysicalDevice> VulkanDriver::createGPUPhysicalDevice(
        const PhysicalGPUDeviceDescription& description) {
        return std::make_unique<VulkanGPUPhysicalDevice>(*this, description);
    }

    std::unique_ptr<IGPUDevice> VulkanDriver::createDevice(
        const GPUDeviceDescription& deviceDescription) {
        return std::make_unique<VulkanDevice>(*this, deviceDescription);
    }

    std::unique_ptr<IRenderSurface> VulkanDriver::createSurface(
        const RenderSurfaceDescription& description) {
        return std::make_unique<VulkanSurface>(*this, description);
    }
}  // namespace aetherion