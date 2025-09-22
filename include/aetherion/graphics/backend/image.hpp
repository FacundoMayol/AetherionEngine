#pragma once

#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"
#include "aetherion/util/common_definitions.hpp"

namespace aetherion {
    struct ImageDescription {
        RenderImageType type = RenderImageType::Tex2d;
        Format format;
        Extent3Du extent;
        uint32_t mipLevels;
        uint32_t arrayLayers;
        SampleCount sampleCount = SampleCount::Count1;
        RenderImageLayout initialLayout = RenderImageLayout::Undefined;
        RenderImageLayout primaryLayout = RenderImageLayout::Undefined;
        RenderImageTiling tiling = RenderImageTiling::Optimal;
        MemoryUsage memoryUsage = MemoryUsage::PreferGpu;
        RenderImageUsageFlags usages;
        SharingMode sharingMode;
        std::vector<uint32_t> queueFamilies;
        bool cubeCompatible = false;
        bool arrayCompatible = false;
    };

    class IRenderImage : public IRenderResource {
      public:
        ~IRenderImage() override = 0;

        IRenderImage(const IRenderImage&) = delete;
        IRenderImage& operator=(const IRenderImage&) = delete;

      protected:
        IRenderImage() = default;
        IRenderImage(IRenderImage&&) noexcept = default;
        IRenderImage& operator=(IRenderImage&&) noexcept = default;
    };
}  // namespace aetherion
