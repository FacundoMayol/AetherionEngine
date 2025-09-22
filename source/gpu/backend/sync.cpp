#include "aetherion/gpu/backend/sync.hpp"

namespace aetherion {
    IGPUFence::~IGPUFence() = default;

    IGPUBinarySemaphore::~IGPUBinarySemaphore() = default;

    IGPUTimelineSemaphore::~IGPUTimelineSemaphore() = default;
}  // namespace aetherion
