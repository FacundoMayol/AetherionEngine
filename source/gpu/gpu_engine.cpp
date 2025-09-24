#include "aetherion/gpu/gpu_engine.hpp"

namespace aetherion {
    GPUEngine::GPUEngine(const GPUDriverDescription& driverDescription,
                         const PhysicalGPUDeviceDescription& physicalDeviceDescription,
                         const QueueSelectionFunctor& queueSelectionFunctor)
        : driver_(IGPUDriver::create(driverDescription)),
          physicalDevice_(driver_->createPhysicalDevice(physicalDeviceDescription)),
          device_(driver_->createDevice(
              {.physicalDevice = physicalDevice_.get(),
               .queueFamilyDescriptions = queueSelectionFunctor(*physicalDevice_)})) {}
}  // namespace aetherion