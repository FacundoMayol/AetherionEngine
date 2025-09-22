#pragma once

#include <vector>

#include "aetherion/gpu/backend/render_definitions.hpp"
#include "aetherion/gpu/backend/resource.hpp"

namespace aetherion {
    struct GPUBufferDescription {
        size_t size;
        MemoryUsage memoryUsage;
        GPUBufferUsageFlags usages;
        AllocationAccessType allocationAccess;
        SharingMode sharingMode;
        std::vector<uint32_t> queueFamilies;
        bool dedicatedAllocation = false;
        bool persistentMapping = false;
    };

    class IGPUBuffer : public IGPUResource {
      public:
        ~IGPUBuffer() override = 0;

        IGPUBuffer(const IGPUBuffer&) = delete;
        IGPUBuffer& operator=(const IGPUBuffer&) = delete;

        virtual void* map() = 0;
        virtual void unmap() = 0;

      protected:
        IGPUBuffer() = default;
        IGPUBuffer(IGPUBuffer&&) noexcept = default;
        IGPUBuffer& operator=(IGPUBuffer&&) noexcept = default;
    };
}  // namespace aetherion
