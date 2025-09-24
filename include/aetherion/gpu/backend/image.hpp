#pragma once

#include <vector>

#include "aetherion/gpu/backend/render_definitions.hpp"
#include "aetherion/gpu/backend/resource.hpp"
#include "aetherion/util/common_definitions.hpp"

namespace aetherion {
    struct GPUImageDescription {
        GPUImageType type = GPUImageType::Tex2d;
        Format format;
        Extent3Du extent;
        uint32_t mipLevels;
        uint32_t arrayLayers;
        SampleCount sampleCount = SampleCount::Count1;
        GPUImageLayout initialLayout = GPUImageLayout::Undefined;
        GPUImageLayout primaryLayout = GPUImageLayout::Undefined;
        GPUImageTiling tiling = GPUImageTiling::Optimal;
        GPUImageUsageFlags usages;
        SharingMode sharingMode;
        std::vector<uint32_t> queueFamilies;
        bool cubeCompatible = false;
        bool arrayCompatible = false;
    };

    class IGPUImage : public IGPUResource {
      public:
        ~IGPUImage() override = 0;

        IGPUImage(const IGPUImage&) = delete;
        IGPUImage& operator=(const IGPUImage&) = delete;

      protected:
        IGPUImage() = default;
        IGPUImage(IGPUImage&&) noexcept = default;
        IGPUImage& operator=(IGPUImage&&) noexcept = default;
    };
}  // namespace aetherion
