#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/image_view.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;
    class VulkanImage;

    class VulkanImageView : public IImageView {
      public:
        VulkanImageView() = delete;
        VulkanImageView(VulkanDevice& device, const ImageViewDescription& description);
        VulkanImageView(VulkanDevice& device, vk::ImageView imageView);
        virtual ~VulkanImageView() noexcept override;

        VulkanImageView(const VulkanImageView&) = delete;
        VulkanImageView& operator=(const VulkanImageView&) = delete;

        VulkanImageView(VulkanImageView&&) noexcept;
        VulkanImageView& operator=(VulkanImageView&&) noexcept;

        inline vk::ImageView getVkImageView() const { return imageView_; }

      private:
        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;

        vk::ImageView imageView_;
    };
}  // namespace aetherion