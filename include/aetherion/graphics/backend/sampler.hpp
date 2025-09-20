#pragma once

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"

namespace aetherion {
    struct SamplerDescription {
        FilterMode magFilter;
        FilterMode minFilter;
        SamplerMipmapMode mipmapMode;
        SamplerAddressMode addressModeU;
        SamplerAddressMode addressModeV;
        SamplerAddressMode addressModeW;
        float mipLodBias;
        bool enableAnisotropicFiltering;
        float maxAnisotropy;
        bool enableCompare;
        CompareOp compareOp;
        float minLod;
        float maxLod;
        SamplerBorderColor borderColor;
        bool unnormalizedCoordinates = false;
    };

    class ISampler : public IResource {
      public:
        virtual ~ISampler() = 0;

        ISampler(const ISampler&) = delete;
        ISampler& operator=(const ISampler&) = delete;

      protected:
        ISampler() = default;
        ISampler(ISampler&&) noexcept = default;
        ISampler& operator=(ISampler&&) noexcept = default;
    };
}  // namespace aetherion
