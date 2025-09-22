#pragma once

#include <VkBootstrap.h>

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/swapchain.hpp"
#include "vulkan_image.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;
    class VulkanImage;

    class VulkanSwapchain : public ISwapchain {
      public:
        VulkanSwapchain() = delete;
        VulkanSwapchain(VulkanDevice& device, const SwapchainDescription& description);
        VulkanSwapchain(vk::Device device, vkb::Swapchain swapchain);
        virtual ~VulkanSwapchain() noexcept override;

        VulkanSwapchain(const VulkanSwapchain&) = delete;
        VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

        VulkanSwapchain(VulkanSwapchain&&) noexcept;
        VulkanSwapchain& operator=(VulkanSwapchain&&) noexcept;

        virtual ResultValue<SwapchainAcquireResultCode, uint32_t> acquireNextImage(
            uint64_t timeout, IGPUBinarySemaphore& semaphore, IGPUFence& fence) override;

        virtual uint32_t getImageCount() const override;

        virtual IImage& getImage(uint32_t index) override;

        inline vk::SwapchainKHR getVkSwapchain() const { return swapchain_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;

        vkb::Swapchain builderSwapchain_;
        vk::SwapchainKHR swapchain_;

        std::vector<VulkanImage> wrappedImages_;
    };
}  // namespace aetherion