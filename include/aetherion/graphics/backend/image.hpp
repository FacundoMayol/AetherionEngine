#pragma once

#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"
#include "aetherion/util/common_definitions.hpp"

namespace aetherion {
    struct ImageDescription {
        ImageType type = ImageType::Tex2d;
        Format format;
        Extent3Du extent;
        uint32_t mipLevels;
        uint32_t arrayLayers;
        SampleCount sampleCount = SampleCount::Count1;
        ImageLayout initialLayout = ImageLayout::Undefined;
        ImageLayout primaryLayout = ImageLayout::Undefined;
        ImageTiling tiling = ImageTiling::Optimal;
        MemoryUsage memoryUsage = MemoryUsage::PreferGpu;
        ImageUsageFlags usages;
        SharingMode sharingMode;
        std::vector<uint32_t> queueFamilies;
        bool cubeCompatible = false;
        bool arrayCompatible = false;
    };

    class IImage : public IResource {
      public:
        ~IImage() override = 0;

        IImage(const IImage&) = delete;
        IImage& operator=(const IImage&) = delete;

      protected:
        IImage() = default;
        IImage(IImage&&) noexcept = default;
        IImage& operator=(IImage&&) noexcept = default;
    };
}  // namespace aetherion
