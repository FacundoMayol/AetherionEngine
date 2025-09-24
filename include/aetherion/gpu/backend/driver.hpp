#pragma once

#include <memory>

namespace aetherion {
    // Forward declarations
    class IGPUPhysicalDevice;
    class IGPUDevice;
    class IWindowManager;
    class IWindow;
    class IRenderSurface;
    struct PhysicalGPUDeviceDescription;
    struct GPUDeviceDescription;
    struct RenderSurfaceDescription;

    enum class DriverType { Vulkan, Direct3D12, Metal, OpenGL, OpenGLES, Software };

    struct GPUDriverDescription {
        DriverType type;
        std::string name;
        std::string version;
        bool validationLayersEnabled = false;
    };

    class IGPUDriver {
      public:
        static std::unique_ptr<IGPUDriver> create(const GPUDriverDescription& description);

        virtual ~IGPUDriver() = 0;

        IGPUDriver(const IGPUDriver&) = delete;
        IGPUDriver& operator=(const IGPUDriver&) = delete;

        virtual std::unique_ptr<IGPUPhysicalDevice> createPhysicalDevice(
            const PhysicalGPUDeviceDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUDevice> createDevice(
            const GPUDeviceDescription& deviceDescription)
            = 0;

        virtual std::unique_ptr<IRenderSurface> createSurface(
            const RenderSurfaceDescription& description)
            = 0;

      protected:
        IGPUDriver() = default;
        IGPUDriver(IGPUDriver&&) noexcept = default;
        IGPUDriver& operator=(IGPUDriver&&) noexcept = default;
    };
}  // namespace aetherion