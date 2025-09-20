#include "vulkan_image_view.hpp"

#include "vulkan_device.hpp"
#include "vulkan_image.hpp"
#include "vulkan_render_definitions.hpp"

namespace aetherion {
    VulkanImageView::VulkanImageView(VulkanDevice& device, const ImageViewDescription& description)
        : device_(&device) {
        if (!description.image) {
            throw std::invalid_argument("Image in ImageViewDescription is null.");
        }

        auto& vkImage = dynamic_cast<VulkanImage&>(*description.image);

        imageView_ = device_->getVkDevice().createImageView(
            vk::ImageViewCreateInfo()
                .setImage(vkImage.getVkImage())
                .setViewType(toVkImageViewType(description.viewType))
                .setFormat(toVkFormat(description.format))
                .setComponents(vk::ComponentMapping()
                                   .setR(toVkComponentSwizzle(description.swizzle.r))
                                   .setG(toVkComponentSwizzle(description.swizzle.g))
                                   .setB(toVkComponentSwizzle(description.swizzle.b))
                                   .setA(toVkComponentSwizzle(description.swizzle.a)))
                .setSubresourceRange(
                    vk::ImageSubresourceRange()
                        .setAspectMask(toVkImageAspectFlags(description.subresource.aspectMask))
                        .setBaseMipLevel(description.subresource.range.baseMipLevel)
                        .setLevelCount(description.subresource.range.layerCount)
                        .setBaseArrayLayer(description.subresource.range.baseArrayLayer)
                        .setLayerCount(description.subresource.range.layerCount)));
    }

    VulkanImageView::VulkanImageView(VulkanDevice& device, vk::ImageView imageView)
        : device_(&device), imageView_(imageView) {}

    VulkanImageView::~VulkanImageView() noexcept { clear(); }

    VulkanImageView::VulkanImageView(VulkanImageView&& other) noexcept
        : IImageView(std::move(other)), device_(other.device_), imageView_(other.imageView_) {
        other.device_ = nullptr;
        other.imageView_ = nullptr;
    }

    VulkanImageView& VulkanImageView::operator=(VulkanImageView&& other) noexcept {
        if (this != &other) {
            clear();

            IImageView::operator=(std::move(other));
            device_ = other.device_;
            imageView_ = other.imageView_;

            other.release();
        }
        return *this;
    }

    void VulkanImageView::clear() noexcept {
        if (imageView_ && device_) {
            device_->getVkDevice().destroyImageView(imageView_);
            imageView_ = nullptr;
        }
        device_ = nullptr;
    }

    void VulkanImageView::release() noexcept {
        imageView_ = nullptr;
        device_ = nullptr;
    }
}  // namespace aetherion