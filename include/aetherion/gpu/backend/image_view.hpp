#pragma once

#include "aetherion/gpu/backend/render_definitions.hpp"
#include "aetherion/gpu/backend/resource.hpp"

namespace aetherion {
    // Forward declarations
    class IGPUImage;

    struct GPUImageViewDescription {
        IGPUImage* image{};
        Format format = Format::Undefined;
        GPUImageViewType viewType = GPUImageViewType::Tex2d;
        struct SwizzleDescription {
            ComponentSwizzle r = ComponentSwizzle::Identity;
            ComponentSwizzle g = ComponentSwizzle::Identity;
            ComponentSwizzle b = ComponentSwizzle::Identity;
            ComponentSwizzle a = ComponentSwizzle::Identity;
        } swizzle;
        GPUImageSubresourceDescription subresource;
    };

    class IGPUImageView : public IGPUResource {
      public:
        ~IGPUImageView() override = 0;

        IGPUImageView(const IGPUImageView&) = delete;
        IGPUImageView& operator=(const IGPUImageView&) = delete;

      protected:
        IGPUImageView() = default;
        IGPUImageView(IGPUImageView&&) noexcept = default;
        IGPUImageView& operator=(IGPUImageView&&) noexcept = default;
    };
}  // namespace aetherion
