#pragma once

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"

namespace aetherion {
    // Forward declarations
    class IRenderImage;

    struct RenderImageViewDescription {
        IRenderImage* image{};
        Format format = Format::Undefined;
        RenderImageViewType viewType = RenderImageViewType::Tex2d;
        struct SwizzleDescription {
            ComponentSwizzle r = ComponentSwizzle::Identity;
            ComponentSwizzle g = ComponentSwizzle::Identity;
            ComponentSwizzle b = ComponentSwizzle::Identity;
            ComponentSwizzle a = ComponentSwizzle::Identity;
        } swizzle;
        RenderImageSubresourceDescription subresource;
    };

    class IRenderImageView : public IRenderResource {
      public:
        ~IRenderImageView() override = 0;

        IRenderImageView(const IRenderImageView&) = delete;
        IRenderImageView& operator=(const IRenderImageView&) = delete;

      protected:
        IRenderImageView() = default;
        IRenderImageView(IRenderImageView&&) noexcept = default;
        IRenderImageView& operator=(IRenderImageView&&) noexcept = default;
    };
}  // namespace aetherion
