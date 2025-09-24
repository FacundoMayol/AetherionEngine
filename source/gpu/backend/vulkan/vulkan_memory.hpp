#pragma once

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

#include "aetherion/gpu/backend/memory.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanAllocator : public IGPUAllocator {
      public:
        VulkanAllocator() = delete;
        VulkanAllocator(VulkanDevice& device, const GPUAllocatorDescription& description);
        VulkanAllocator(vk::PhysicalDevice physicalDevice, vk::Device device,
                        vma::Allocator allocator);
        ~VulkanAllocator() noexcept override;

        VulkanAllocator(const VulkanAllocator&) = delete;
        VulkanAllocator& operator=(const VulkanAllocator&) = delete;

        VulkanAllocator(VulkanAllocator&&) noexcept;
        VulkanAllocator& operator=(VulkanAllocator&&) noexcept;

        std::unique_ptr<IGPUAllocatorPool> createPool(
            const GPUAllocatorPoolDescription& description) override;

        std::unique_ptr<IGPUAllocation> allocate(
            const GPUAllocationMemoryRequirementsDescription& memoryRequirements,
            const GPUAllocationDescription& description) override;

        std::unique_ptr<IGPUAllocation> allocateForImage(
            IGPUImage& image, const GPUAllocationDescription& description) override;

        std::unique_ptr<IGPUAllocation> allocateForBuffer(
            IGPUBuffer& buffer, const GPUAllocationDescription& description) override;

        std::unique_ptr<IGPUImage> createImage(
            const GPUImageDescription& description,
            const GPUAllocationDescription& allocationDescription) override;

        std::unique_ptr<IGPUBuffer> createBuffer(
            const GPUBufferDescription& description,
            const GPUAllocationDescription& allocationDescription) override;

        std::unique_ptr<IGPUImage> createAliasedImage(IGPUAllocation& allocation,
                                                      const GPUImageDescription& description,
                                                      size_t offset = 0) override;

        std::unique_ptr<IGPUBuffer> createAliasedBuffer(IGPUAllocation& allocation,
                                                        const GPUBufferDescription& description,
                                                        size_t offset = 0) override;

        inline vk::PhysicalDevice getVkPhysicalDevice() const { return physicalDevice_; }

        inline vk::Device getVkDevice() const { return device_; }

        inline vma::Allocator getVmaAllocator() const { return allocator_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::PhysicalDevice physicalDevice_;
        vk::Device device_;
        vma::Allocator allocator_;
    };

    class VulkanAllocation : public IGPUAllocation {
      public:
        VulkanAllocation() = delete;
        VulkanAllocation(VulkanAllocator& allocator,
                         const GPUAllocationMemoryRequirementsDescription& memoryRequirements,
                         const GPUAllocationDescription& description);
        VulkanAllocation(VulkanAllocator& allocator, const IGPUImage& image,
                         const GPUAllocationDescription& description);
        VulkanAllocation(VulkanAllocator& allocator, const IGPUBuffer& buffer,
                         const GPUAllocationDescription& description);
        VulkanAllocation(vma::Allocator allocator, vma::Allocation allocation);
        ~VulkanAllocation() noexcept override;

        VulkanAllocation(const VulkanAllocation&) = delete;
        VulkanAllocation& operator=(const VulkanAllocation&) = delete;

        VulkanAllocation(VulkanAllocation&&) noexcept;
        VulkanAllocation& operator=(VulkanAllocation&&) noexcept;

        inline vma::Allocation getVmaAllocation() const { return allocation_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vma::Allocator allocator_;
        vma::Allocation allocation_;
    };

    class VulkanAllocatorPool : public IGPUAllocatorPool {
      public:
        VulkanAllocatorPool() = delete;
        VulkanAllocatorPool(VulkanAllocator& allocator,
                            const GPUAllocatorPoolDescription& description);
        VulkanAllocatorPool(vma::Allocator allocator, vma::Pool pool);
        ~VulkanAllocatorPool() noexcept override;

        VulkanAllocatorPool(const VulkanAllocatorPool&) = delete;
        VulkanAllocatorPool& operator=(const VulkanAllocatorPool&) = delete;

        VulkanAllocatorPool(VulkanAllocatorPool&&) noexcept;
        VulkanAllocatorPool& operator=(VulkanAllocatorPool&&) noexcept;

        inline vma::Pool getVmaPool() const { return pool_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vma::Allocator allocator_;
        vma::Pool pool_;
    };
}  // namespace aetherion