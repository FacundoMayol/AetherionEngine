#include "vulkan_buffer.hpp"

#include "vulkan_device.hpp"
#include "vulkan_render_definitions.hpp"

namespace aetherion {
    VulkanBuffer::VulkanBuffer(VulkanDevice& device, const BufferDescription& description)
        : device_(&device) {
        vma::AllocationCreateFlags allocationFlags
            = toVkAllocationCreateFlags(description.allocationAccess);

        if (description.dedicatedAllocation) {
            allocationFlags |= vma::AllocationCreateFlagBits::eDedicatedMemory;
        }

        if (description.persistentMapping) {
            allocationFlags |= vma::AllocationCreateFlagBits::eMapped;
        }

        std::tie(buffer_, allocation_) = device_->getVmaAllocator().createBuffer(
            vk::BufferCreateInfo()
                .setSize(description.size)
                .setUsage(toVkBufferUsageFlags(description.usages))
                .setSharingMode(toVkSharingMode(description.sharingMode))
                .setQueueFamilyIndices(description.queueFamilies),
            vma::AllocationCreateInfo()
                .setUsage(toVmaMemoryUsage(description.memoryUsage))
                .setFlags(allocationFlags));
    }

    VulkanBuffer::VulkanBuffer(VulkanDevice& device, vk::Buffer buffer, vma::Allocation allocation)
        : device_(&device), buffer_(buffer), allocation_(allocation) {}

    VulkanBuffer::~VulkanBuffer() noexcept { clear(); }

    VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
        : IBuffer(std::move(other)),
          device_(other.device_),
          buffer_(other.buffer_),
          allocation_(other.allocation_) {
        other.device_ = nullptr;
        other.buffer_ = nullptr;
        other.allocation_ = nullptr;
    }

    VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& other) noexcept {
        if (this != &other) {
            clear();

            IBuffer::operator=(std::move(other));
            device_ = other.device_;
            buffer_ = other.buffer_;
            allocation_ = other.allocation_;

            other.release();
        }
        return *this;
    }

    void VulkanBuffer::clear() noexcept {
        if (buffer_ && allocation_ && device_) {
            device_->getVmaAllocator().destroyBuffer(buffer_, allocation_);
            buffer_ = nullptr;
            allocation_ = nullptr;
        }
        device_ = nullptr;
    }

    void VulkanBuffer::release() noexcept {
        buffer_ = nullptr;
        allocation_ = nullptr;
        device_ = nullptr;
    }

    void* VulkanBuffer::map() {
        void* data = nullptr;
        if (device_->getVmaAllocator().mapMemory(allocation_, &data) != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to map Vulkan buffer memory.");
        }
        return data;
    }

    void VulkanBuffer::unmap() { device_->getVmaAllocator().unmapMemory(allocation_); }
}  // namespace aetherion