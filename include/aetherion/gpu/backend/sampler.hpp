#pragma once

#include "aetherion/gpu/backend/render_definitions.hpp"
#include "aetherion/gpu/backend/resource.hpp"

namespace aetherion {
    struct SamplerDescription {
        FilterMode magFilter = FilterMode::Nearest;
        FilterMode minFilter = FilterMode::Nearest;
        SamplerMipmapMode mipmapMode = SamplerMipmapMode::Nearest;
        SamplerAddressMode addressModeU = SamplerAddressMode::Repeat;
        SamplerAddressMode addressModeV = SamplerAddressMode::Repeat;
        SamplerAddressMode addressModeW = SamplerAddressMode::Repeat;
        float mipLodBias{};
        bool enableAnisotropicFiltering{};
        float maxAnisotropy{};
        bool enableCompare{};
        CompareOp compareOp = CompareOp::Always;
        float minLod{};
        float maxLod{};
        SamplerBorderColor borderColor = SamplerBorderColor::FloatTransparentBlack;
        bool unnormalizedCoordinates = false;
    };

    class ISampler : public IGPUResource {
      public:
        ~ISampler() override = 0;

        ISampler(const ISampler&) = delete;
        ISampler& operator=(const ISampler&) = delete;

      protected:
        ISampler() = default;
        ISampler(ISampler&&) noexcept = default;
        ISampler& operator=(ISampler&&) noexcept = default;
    };
}  // namespace aetherion
