#include "aetherion/gpu/backend/memory.hpp"

namespace aetherion {
    IGPUAllocator::~IGPUAllocator() = default;

    IGPUAllocation::~IGPUAllocation() = default;

    IGPUAllocatorPool::~IGPUAllocatorPool() = default;
}  // namespace aetherion