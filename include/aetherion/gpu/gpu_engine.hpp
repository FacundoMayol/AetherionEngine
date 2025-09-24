#pragma once

#include <functional>

#include "aetherion/gpu/backend/device.hpp"
#include "aetherion/gpu/backend/driver.hpp"

namespace aetherion {
    using QueueSelectionFunctor = std::function<std::vector<GPUQueueFamilyDescription>(
        const IGPUPhysicalDevice& physicalDevice)>;

    class GPUEngine {
      public:
        GPUEngine(const GPUDriverDescription& driverDescription,
                  const PhysicalGPUDeviceDescription& physicalDeviceDescription,
                  const QueueSelectionFunctor& queueSelectionFunctor);
        ~GPUEngine() noexcept = default;

        GPUEngine(const GPUEngine&) = delete;
        GPUEngine& operator=(const GPUEngine&) = delete;

        GPUEngine(GPUEngine&&) = delete;
        GPUEngine& operator=(GPUEngine&&) = delete;

        IGPUDriver& getDriver();
        const IGPUDriver& getDriver() const;

        IGPUPhysicalDevice& getPhysicalDevice();
        const IGPUPhysicalDevice& getPhysicalDevice() const;

        IGPUDevice& getDevice();
        const IGPUDevice& getDevice() const;

      private:
        std::unique_ptr<IGPUDriver> driver_;
        std::unique_ptr<IGPUPhysicalDevice> physicalDevice_;
        std::unique_ptr<IGPUDevice> device_;
    };
}  // namespace aetherion