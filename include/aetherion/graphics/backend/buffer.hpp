#pragma once

#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"

namespace aetherion {
    struct BufferDescription {
        size_t size;
        MemoryUsage memoryUsage;
        RenderBufferUsageFlags usages;
        AllocationAccessType allocationAccess;
        SharingMode sharingMode;
        std::vector<uint32_t> queueFamilies;
        bool dedicatedAllocation = false;
        bool persistentMapping = false;
    };

    class IRenderBuffer : public IRenderResource {
      public:
        ~IRenderBuffer() override = 0;

        IRenderBuffer(const IRenderBuffer&) = delete;
        IRenderBuffer& operator=(const IRenderBuffer&) = delete;

        virtual void* map() = 0;
        virtual void unmap() = 0;

      protected:
        IRenderBuffer() = default;
        IRenderBuffer(IRenderBuffer&&) noexcept = default;
        IRenderBuffer& operator=(IRenderBuffer&&) noexcept = default;
    };
}  // namespace aetherion
