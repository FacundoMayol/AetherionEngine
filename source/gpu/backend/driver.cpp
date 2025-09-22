#include "aetherion/gpu/backend/driver.hpp"

#include "vulkan/vulkan_driver.hpp"

namespace aetherion {
    std::unique_ptr<IGPUDriver> IGPUDriver::create(const GPUDriverDescription& description) {
        switch (description.type) {
            case DriverType::Vulkan:
                return std::make_unique<VulkanDriver>(description);
            // case DriverType::Direct3D12:
            //     return std::make_unique<Direct3D12Driver>(description);
            // case DriverType::Metal:
            //     return std::make_unique<MetalDriver>(description);
            // case DriverType::OpenGL:
            //     return std::make_unique<OpenGLDriver>(description);
            // case DriverType::OpenGLES:
            //     return std::make_unique<OpenGLESDriver>(description);
            // case DriverType::Software:
            //     return std::make_unique<SoftwareDriver>(description);
            default:
                throw std::invalid_argument("Unsupported DriverType");
        }
    }

    IGPUDriver::~IGPUDriver() = default;
}  // namespace aetherion