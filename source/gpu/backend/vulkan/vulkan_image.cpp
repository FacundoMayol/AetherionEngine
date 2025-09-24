#include "vulkan_image.hpp"

#include "vulkan_device.hpp"
#include "vulkan_render_definitions.hpp"

namespace aetherion {
    VulkanImage::VulkanImage(VulkanDevice& device, const GPUImageDescription& description)
        : device_(device.getVkDevice()) {
        image_ = device_.createImage(
            vk::ImageCreateInfo()
                .setImageType(toVkImageType(description.type))
                .setFormat(toVkFormat(description.format))
                .setExtent(toVkExtent3D(description.extent))
                .setMipLevels(description.mipLevels)
                .setArrayLayers(description.arrayLayers)
                .setSamples(toVkSampleCount(description.sampleCount))
                .setInitialLayout(toVkImageLayout(description.initialLayout))
                .setTiling(toVkImageTiling(description.tiling))
                .setUsage(toVkImageUsageFlags(description.usages))
                .setSharingMode(toVkSharingMode(description.sharingMode))
                .setQueueFamilyIndices(description.queueFamilies)
                .setFlags(
                    (description.cubeCompatible ? vk::ImageCreateFlagBits::eCubeCompatible
                                                : vk::ImageCreateFlags())
                    | (description.arrayCompatible ? vk::ImageCreateFlagBits::e2DArrayCompatible
                                                   : vk::ImageCreateFlags())
                    | (description.type == GPUImageType::Tex3d
                           ? vk::ImageCreateFlagBits::e2DArrayCompatible  // Assuming this usage.
                           : vk::ImageCreateFlags())));
    }

    VulkanImage::VulkanImage(VulkanAllocator& allocator, const GPUImageDescription& description,
                             const GPUAllocationDescription& allocationDescription)
        : device_(allocator.getVkDevice()), allocator_(allocator.getVmaAllocator()) {
        auto* vkAllocatorPool = dynamic_cast<VulkanAllocatorPool*>(allocationDescription.pool);

        std::tie(image_, allocation_) = allocator_.createImage(
            vk::ImageCreateInfo()
                .setImageType(toVkImageType(description.type))
                .setFormat(toVkFormat(description.format))
                .setExtent(toVkExtent3D(description.extent))
                .setMipLevels(description.mipLevels)
                .setArrayLayers(description.arrayLayers)
                .setSamples(toVkSampleCount(description.sampleCount))
                .setInitialLayout(toVkImageLayout(description.initialLayout))
                .setTiling(toVkImageTiling(description.tiling))
                .setUsage(toVkImageUsageFlags(description.usages))
                .setSharingMode(toVkSharingMode(description.sharingMode))
                .setQueueFamilyIndices(description.queueFamilies)
                .setFlags(
                    (description.cubeCompatible ? vk::ImageCreateFlagBits::eCubeCompatible
                                                : vk::ImageCreateFlags())
                    | (description.arrayCompatible ? vk::ImageCreateFlagBits::e2DArrayCompatible
                                                   : vk::ImageCreateFlags())
                    | (description.type == GPUImageType::Tex3d
                           ? vk::ImageCreateFlagBits::e2DArrayCompatible  // Assuming this usage.
                           : vk::ImageCreateFlags())),
            vma::AllocationCreateInfo()
                //.setMemoryTypeBits(allocationDescription.memoryTypeBits)
                .setPool(vkAllocatorPool ? vkAllocatorPool->getVmaPool() : nullptr)
                .setPriority(allocationDescription.priority)
                .setUsage(toVmaMemoryUsage(allocationDescription.memoryUsage))
                .setFlags(toVmaAllocationCreateFlags(allocationDescription.properties)));
    }

    VulkanImage::VulkanImage(vk::Device device, vma::Allocator allocator, vk::Image image,
                             vma::Allocation allocation)
        : device_(device), allocator_(allocator), image_(image), allocation_(allocation) {}

    VulkanImage::~VulkanImage() noexcept { clear(); }

    VulkanImage::VulkanImage(VulkanImage&& other) noexcept
        : IGPUImage(std::move(other)),
          device_(other.device_),
          image_(other.image_),
          allocation_(other.allocation_) {
        other.device_ = nullptr;
        other.image_ = nullptr;
        other.allocation_ = nullptr;
    }

    VulkanImage& VulkanImage::operator=(VulkanImage&& other) noexcept {
        if (this != &other) {
            clear();

            IGPUImage::operator=(std::move(other));
            device_ = other.device_;
            image_ = other.image_;
            allocation_ = other.allocation_;

            other.release();
        }
        return *this;
    }

    void VulkanImage::clear() noexcept {
        if (image_ && allocation_ && device_ && allocator_) {
            allocator_.destroyImage(image_, allocation_);
        } else if (image_ && device_) {
            device_.destroyImage(image_);
        }
        release();
    }

    void VulkanImage::release() noexcept {
        image_ = nullptr;
        allocation_ = nullptr;
        device_ = nullptr;
        allocator_ = nullptr;
    }
}  // namespace aetherion