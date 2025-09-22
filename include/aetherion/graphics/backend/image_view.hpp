#pragma once

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"

namespace aetherion {
    // Forward declarations
    class IImage;

    struct ImageViewDescription {
        IImage* image{};
        Format format = Format::Undefined;
        ImageViewType viewType = ImageViewType::Tex2d;
        struct SwizzleDescription {
            ComponentSwizzle r = ComponentSwizzle::Identity;
            ComponentSwizzle g = ComponentSwizzle::Identity;
            ComponentSwizzle b = ComponentSwizzle::Identity;
            ComponentSwizzle a = ComponentSwizzle::Identity;
        } swizzle;
        ImageSubresourceDescription subresource;
    };

    class IImageView : public IResource {
      public:
        ~IImageView() override = 0;

        IImageView(const IImageView&) = delete;
        IImageView& operator=(const IImageView&) = delete;

      protected:
        IImageView() = default;
        IImageView(IImageView&&) noexcept = default;
        IImageView& operator=(IImageView&&) noexcept = default;
    };
}  // namespace aetherion
