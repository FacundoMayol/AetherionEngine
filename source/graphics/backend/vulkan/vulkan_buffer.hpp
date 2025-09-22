#pragma once

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/buffer.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanBuffer : public IBuffer {
      public:
        VulkanBuffer() = delete;
        VulkanBuffer(VulkanDevice& device, const BufferDescription& description);
        VulkanBuffer(vk::Device device, vma::Allocator allocator, vk::Buffer buffer,
                     vma::Allocation allocation);
        virtual ~VulkanBuffer() noexcept override;

        VulkanBuffer(const VulkanBuffer&) = delete;
        VulkanBuffer& operator=(const VulkanBuffer&) = delete;

        VulkanBuffer(VulkanBuffer&&) noexcept;
        VulkanBuffer& operator=(VulkanBuffer&&) noexcept;

        virtual void* map() override;
        virtual void unmap() override;

        inline vk::Buffer getVkBuffer() const { return buffer_; }
        inline vma::Allocation getVmaAllocation() const { return allocation_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;
        vma::Allocator allocator_;

        vk::Buffer buffer_;
        vma::Allocation allocation_;
    };
}  // namespace aetherion