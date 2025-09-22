#pragma once

#include "aetherion/graphics/backend/device.hpp"
#include "aetherion/graphics/backend/driver.hpp"

namespace aetherion {
    class RenderEngine {
      public:
        /*RenderEngine(const RenderDriverDescription& driverDescription,
                     const PhysicalDeviceDescription& physicalDeviceDescription,
                     const DeviceDescription& deviceDescription);*/

      private:
        std::unique_ptr<IRenderDriver> driver_;
        std::unique_ptr<IRenderDevice> device_;
    };
}  // namespace aetherion