#include "vulkan_swapchain.hpp"

#include <fmt/core.h>

#include "vulkan_device.hpp"
#include "vulkan_render_definitions.hpp"
#include "vulkan_surface.hpp"
#include "vulkan_sync.hpp"

namespace aetherion {
    void populateSwapchainImages(vk::Device device, vkb::Swapchain swapchain,
                                 std::vector<VulkanImage>& wrappedImages) {
        auto vkImagesResult = swapchain.get_images();

        if (!vkImagesResult) {
            throw(std::runtime_error(fmt::format("Failed to get Vulkan swapchain images ({})",
                                                 vkImagesResult.error().message())));
        }

        auto vkImages = vkImagesResult.value();

        wrappedImages.clear();
        wrappedImages.reserve(vkImages.size());

        for (const auto& vkImage : vkImages) {
            wrappedImages.emplace_back(device, nullptr, vkImage, nullptr);
        }
    }

    VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, const SwapchainDescription& description)
        : device_(device.getVkDevice()) {
        if (!description.surface) {
            throw(std::invalid_argument("A surface is required to create a swapchain."));
        }
        auto vkSurface = dynamic_cast<const VulkanSurface&>(*description.surface).getVkSurface();

        const auto& vulkanBuilderSwapchainResult
            = vkb::SwapchainBuilder(device.getVkBuilderDevice(), vkSurface)
                  .set_desired_min_image_count(description.minImageCount)
                  .set_desired_format(
                      vk::SurfaceFormatKHR(toVkFormat(description.surfaceFormat.imageFormat),
                                           toVkColorSpace(description.surfaceFormat.colorSpace)))
                  .set_desired_extent(description.extent.width, description.extent.height)
                  .set_image_array_layer_count(description.arrayLayers)
                  .set_image_usage_flags(
                      static_cast<VkImageUsageFlags>(toVkImageUsageFlags(description.imageUsages)))
                  // VKB uses present and graphics queue on exclusive mode. TODO: VKB is not ideal.
                  // .set_sharing_mode(toVkSharingMode(description.sharingMode))
                  // .set_queue_family_indices(description.queueFamilyIndices)
                  .set_desired_present_mode(
                      static_cast<VkPresentModeKHR>(toVkPresentMode(description.presentMode)))
                  .build();

        if (!vulkanBuilderSwapchainResult) {
            throw(std::runtime_error(fmt::format("Failed to create Vulkan swapchain ({})",
                                                 vulkanBuilderSwapchainResult.error().message())));
        }

        builderSwapchain_ = vulkanBuilderSwapchainResult.value();
        swapchain_ = vk::SwapchainKHR(builderSwapchain_.swapchain);

        populateSwapchainImages(device_, builderSwapchain_, wrappedImages_);
    }

    VulkanSwapchain::VulkanSwapchain(vk::Device device, vkb::Swapchain swapchain)
        : device_(device), builderSwapchain_(swapchain), swapchain_(swapchain.swapchain) {
        populateSwapchainImages(device_, builderSwapchain_, wrappedImages_);
    }

    VulkanSwapchain::~VulkanSwapchain() noexcept { clear(); }

    VulkanSwapchain::VulkanSwapchain(VulkanSwapchain&& other) noexcept
        : ISwapchain(std::move(other)),
          builderSwapchain_(std::move(other.builderSwapchain_)),
          swapchain_(other.swapchain_),
          wrappedImages_(std::move(other.wrappedImages_)),
          device_(other.device_) {
        other.swapchain_ = nullptr;
        other.device_ = nullptr;
    }

    VulkanSwapchain& VulkanSwapchain::operator=(VulkanSwapchain&& other) noexcept {
        if (this != &other) {
            clear();

            ISwapchain::operator=(std::move(other));
            builderSwapchain_ = std::move(other.builderSwapchain_);
            swapchain_ = other.swapchain_;
            wrappedImages_ = std::move(other.wrappedImages_);
            device_ = other.device_;

            other.release();
        }
        return *this;
    }

    void VulkanSwapchain::clear() noexcept {
        if (builderSwapchain_) {
            vkb::destroy_swapchain(builderSwapchain_);
            builderSwapchain_ = {};
        }
        for (auto& image : wrappedImages_) {
            image.release();  // Prevent double free since images are owned by the swapchain.
        }
        wrappedImages_.clear();
        swapchain_ = nullptr;
        device_ = nullptr;
    }

    void VulkanSwapchain::release() noexcept {
        builderSwapchain_ = {};
        for (auto& image : wrappedImages_) {
            image.release();  // Prevent double free since images are owned by the swapchain.
        }
        wrappedImages_.clear();
        swapchain_ = nullptr;
        device_ = nullptr;
    }

    ResultValue<SwapchainAcquireResultCode, uint32_t> VulkanSwapchain::acquireNextImage(
        uint64_t timeout, IGPUBinarySemaphore& semaphore, IGPUFence& fence) {
        auto& vkSemaphore = dynamic_cast<VulkanBinarySemaphore&>(semaphore);
        auto& vkFence = dynamic_cast<VulkanFence&>(fence);

        const auto result = device_.acquireNextImageKHR(
            swapchain_, timeout, vkSemaphore.getVkSemaphore(), vkFence.getVkFence());

        auto resultCode = toSwapchainAcquireResultCode(result.result);

        if (result.result == vk::Result::eSuccess || result.result == vk::Result::eSuboptimalKHR) {
            return ResultValue<SwapchainAcquireResultCode, uint32_t>(result.value, resultCode);
        } else {
            return ResultValue<SwapchainAcquireResultCode, uint32_t>(resultCode);
        }
    }

    uint32_t VulkanSwapchain::getImageCount() const {
        return static_cast<uint32_t>(wrappedImages_.size());
    }

    IRenderImage& VulkanSwapchain::getImage(uint32_t index) { return wrappedImages_.at(index); }
}  // namespace aetherion