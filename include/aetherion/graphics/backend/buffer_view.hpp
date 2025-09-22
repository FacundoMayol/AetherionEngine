#pragma once

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"

namespace aetherion {
    // Forward declarations
    class IRenderBuffer;

    struct RenderBufferViewDescription {
        IRenderBuffer* buffer{};
        Format format = Format::Undefined;  // NOTE: Ignored if buffer is not a texel buffer.
        uint32_t offset = 0;
        uint32_t range = 0;
    };

    class IRenderBufferView : public IRenderResource {
      public:
        ~IRenderBufferView() override = 0;

        IRenderBufferView(const IRenderBufferView&) = delete;
        IRenderBufferView& operator=(const IRenderBufferView&) = delete;

      protected:
        IRenderBufferView() = default;
        IRenderBufferView(IRenderBufferView&&) noexcept = default;
        IRenderBufferView& operator=(IRenderBufferView&&) noexcept = default;
    };
}  // namespace aetherion
