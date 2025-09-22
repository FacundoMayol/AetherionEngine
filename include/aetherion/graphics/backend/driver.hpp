#pragma once

#include <memory>

namespace aetherion {
    // Forward declarations
    class IPhysicalDevice;
    class IRenderDevice;
    class IWindowManager;
    class IWindow;
    class IRenderSurface;
    struct PhysicalDeviceDescription;
    struct DeviceDescription;
    struct RenderSurfaceDescription;

    enum class DriverType { Vulkan, Direct3D12, Metal, OpenGL, OpenGLES, Software };

    struct RenderDriverDescription {
        DriverType type;
        std::string name;
        std::string version;
        bool validationLayersEnabled = false;
    };

    class IRenderDriver {
      public:
        static std::unique_ptr<IRenderDriver> create(const RenderDriverDescription& description);

        virtual ~IRenderDriver() = 0;

        IRenderDriver(const IRenderDriver&) = delete;
        IRenderDriver& operator=(const IRenderDriver&) = delete;

        virtual std::unique_ptr<IPhysicalDevice> createPhysicalDevice(
            const PhysicalDeviceDescription& description)
            = 0;

        virtual std::unique_ptr<IRenderDevice> createDevice(
            const DeviceDescription& deviceDescription)
            = 0;

        virtual std::unique_ptr<IRenderSurface> createSurface(
            const RenderSurfaceDescription& description)
            = 0;

      protected:
        IRenderDriver() = default;
        IRenderDriver(IRenderDriver&&) noexcept = default;
        IRenderDriver& operator=(IRenderDriver&&) noexcept = default;
    };
}  // namespace aetherion