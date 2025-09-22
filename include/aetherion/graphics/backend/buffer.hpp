#pragma once

#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"

namespace aetherion {
    struct BufferDescription {
        size_t size;
        MemoryUsage memoryUsage;
        BufferUsageFlags usages;
        AllocationAccessType allocationAccess;
        SharingMode sharingMode;
        std::vector<uint32_t> queueFamilies;
        bool dedicatedAllocation = false;
        bool persistentMapping = false;
    };

    class IBuffer : public IResource {
      public:
        ~IBuffer() override = 0;

        IBuffer(const IBuffer&) = delete;
        IBuffer& operator=(const IBuffer&) = delete;

        virtual void* map() = 0;
        virtual void unmap() = 0;

      protected:
        IBuffer() = default;
        IBuffer(IBuffer&&) noexcept = default;
        IBuffer& operator=(IBuffer&&) noexcept = default;
    };

}  // namespace aetherion
