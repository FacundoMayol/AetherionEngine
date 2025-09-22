#include "vulkan_surface.hpp"

#include "vulkan_device.hpp"
#include "vulkan_driver.hpp"
#include "vulkan_render_definitions.hpp"

namespace aetherion {
    VulkanSurface::VulkanSurface(VulkanDriver& driver, const SurfaceDescription& description)
        : instance_(driver.getVkInstance()) {
        if (!description.window) {
            throw(std::invalid_argument("A window is required to create a Vulkan surface."));
        }

        surface_ = description.window->createVulkanSurface(instance_);
    }

    VulkanSurface::VulkanSurface(vk::Instance instance, vk::SurfaceKHR surface)
        : instance_(instance), surface_(surface) {}

    VulkanSurface::~VulkanSurface() noexcept { clear(); }

    VulkanSurface::VulkanSurface(VulkanSurface&& other) noexcept
        : ISurface(std::move(other)), instance_(other.instance_), surface_(other.surface_) {
        other.surface_ = nullptr;
        other.instance_ = nullptr;
    }

    void VulkanSurface::clear() noexcept {
        if (surface_ && instance_) {
            instance_.destroySurfaceKHR(surface_);
            surface_ = nullptr;
        }
        instance_ = nullptr;
    }

    void VulkanSurface::release() noexcept {
        surface_ = nullptr;
        instance_ = nullptr;
    }

    VulkanSurface& VulkanSurface::operator=(VulkanSurface&& other) noexcept {
        if (this != &other) {
            clear();

            ISurface::operator=(std::move(other));
            instance_ = other.instance_;
            surface_ = other.surface_;

            other.release();
        }
        return *this;
    }

    std::vector<SurfaceFormat> VulkanSurface::getSupportedFormats(
        const IPhysicalDevice& physicalDevice) const {
        const auto& vkPhysicalDevice
            = dynamic_cast<const VulkanPhysicalDevice&>(physicalDevice).getVkPhysicalDevice();

        auto vkSurfaceFormats = vkPhysicalDevice.getSurfaceFormatsKHR(surface_);

        std::vector<SurfaceFormat> supportedFormats;
        for (const auto& vkSurfaceFormat : vkSurfaceFormats) {
            supportedFormats.push_back(toSurfaceFormat(vkSurfaceFormat));
        }
        return supportedFormats;
    }
}  // namespace aetherion