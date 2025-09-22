#pragma once

#include "aetherion/gpu/backend/device.hpp"
#include "aetherion/gpu/backend/driver.hpp"

namespace aetherion {
    class RenderEngine {
      public:
        RenderEngine(const GPUDriverDescription& driverDescription,
                     const PhysicalGPUDeviceDescription& physicalGPUDeviceDescription,
                     const GPUDeviceDescription& deviceDescription);

      private:
        std::unique_ptr<IGPUDriver> driver_;
        std::unique_ptr<IGPUDevice> device_;
        std::unique_ptr<IGPUQueue> queue_;
    };
}  // namespace aetherion