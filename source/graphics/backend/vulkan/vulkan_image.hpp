#pragma once

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/image.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanImage : public IImage {
      public:
        VulkanImage() = delete;
        VulkanImage(VulkanDevice& device, const ImageDescription& description);
        VulkanImage(VulkanDevice& device, vk::Image image, vma::Allocation allocation);
        virtual ~VulkanImage() noexcept override;

        VulkanImage(const VulkanImage&) = delete;
        VulkanImage& operator=(const VulkanImage&) = delete;

        VulkanImage(VulkanImage&&) noexcept;
        VulkanImage& operator=(VulkanImage&&) noexcept;

        inline vk::Image getVkImage() const { return image_; }
        // NOTE: May be nullptr if no allocation was made (e.g. swapchain images)
        inline vma::Allocation getVmaAllocation() const { return allocation_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;

        vk::Image image_;
        vma::Allocation allocation_;
    };
}  // namespace aetherion