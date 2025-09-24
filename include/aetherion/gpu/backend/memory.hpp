#pragma once

#include <memory>

#include "render_definitions.hpp"

namespace aetherion {
    // Forward declarations
    class IGPUDevice;
    class IGPUAllocatorPool;
    class IGPUAllocation;
    class IGPUImage;
    class IGPUBuffer;
    struct GPUImageDescription;
    struct GPUBufferDescription;

    struct GPUAllocatorDescription {
        AllocatorPropertyFlags flags = AllocatorProperty::None;
        // size_t heapSizeLimit = NO_HEAP_SIZE_LIMIT;
    };

    struct GPUAllocationMemoryRequirementsDescription {
        size_t size = 0;
        size_t alignment = 0;
        uint32_t memoryTypeBits = 0;  // NOTE: 0 = no preference
    };

    struct GPUAllocationDescription {
        AllocationPropertyFlags properties = AllocationProperty::None;
        MemoryUsage memoryUsage = MemoryUsage::PreferGpu;
        IGPUAllocatorPool* pool{};  // NOTE: If null, the default pool is used.
        float priority = 0.0f;      // NOTE: Range [0.0, 1.0].
    };

    struct GPUAllocatorPoolDescription {
        // uint32_t memoryTypeIndex = 0;  // 0 = use default memory type
        AllocatorPoolPropertyFlags flags = AllocatorPoolProperty::None;
        size_t blockSize = 0;      // NOTE: 0 = use default block size
        size_t minBlockCount = 0;  // NOTE: 0 = no minimum
        size_t maxBlockCount = 0;  // NOTE: 0 = no limit
        float priority = 0.0f;
        size_t minAllocationAlignment = 0;  // NOTE: 0 = no minimum
    };

    class IGPUAllocator {
      public:
        virtual ~IGPUAllocator() = 0;

        IGPUAllocator(const IGPUAllocator&) = delete;
        IGPUAllocator& operator=(const IGPUAllocator&) = delete;

        virtual std::unique_ptr<IGPUAllocatorPool> createPool(
            const GPUAllocatorPoolDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUAllocation> allocate(
            const GPUAllocationMemoryRequirementsDescription& memoryRequirements,
            const GPUAllocationDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUAllocation> allocateForImage(
            IGPUImage& image, const GPUAllocationDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUAllocation> allocateForBuffer(
            IGPUBuffer& buffer, const GPUAllocationDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUImage> createImage(
            const GPUImageDescription& description,
            const GPUAllocationDescription& allocationDescription)
            = 0;

        virtual std::unique_ptr<IGPUBuffer> createBuffer(
            const GPUBufferDescription& description,
            const GPUAllocationDescription& allocationDescription)
            = 0;

        virtual std::unique_ptr<IGPUImage> createAliasedImage(
            IGPUAllocation& allocation, const GPUImageDescription& description, size_t offset = 0)
            = 0;

        virtual std::unique_ptr<IGPUBuffer> createAliasedBuffer(
            IGPUAllocation& allocation, const GPUBufferDescription& description, size_t offset = 0)
            = 0;

      protected:
        IGPUAllocator() = default;
        IGPUAllocator(IGPUAllocator&&) noexcept = default;
        IGPUAllocator& operator=(IGPUAllocator&&) noexcept = default;
    };

    class IGPUAllocation {
      public:
        virtual ~IGPUAllocation() = 0;

        IGPUAllocation(const IGPUAllocation&) = delete;
        IGPUAllocation& operator=(const IGPUAllocation&) = delete;

      protected:
        IGPUAllocation() = default;
        IGPUAllocation(IGPUAllocation&&) noexcept = default;
        IGPUAllocation& operator=(IGPUAllocation&&) noexcept = default;
    };

    class IGPUAllocatorPool {
      public:
        virtual ~IGPUAllocatorPool() = 0;

        IGPUAllocatorPool(const IGPUAllocatorPool&) = delete;
        IGPUAllocatorPool& operator=(const IGPUAllocatorPool&) = delete;

      protected:
        IGPUAllocatorPool() = default;
        IGPUAllocatorPool(IGPUAllocatorPool&&) noexcept = default;
        IGPUAllocatorPool& operator=(IGPUAllocatorPool&&) noexcept = default;
    };
}  // namespace aetherion