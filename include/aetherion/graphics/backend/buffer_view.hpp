#pragma once

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"

namespace aetherion {
    // Forward declarations
    class IBuffer;

    struct BufferViewDescription {
        IBuffer* buffer{};
        Format format = Format::Undefined;  // NOTE: Ignored if buffer is not a texel buffer.
        uint32_t offset = 0;
        uint32_t range = 0;
    };

    class IBufferView : public IResource {
      public:
        ~IBufferView() override = 0;

        IBufferView(const IBufferView&) = delete;
        IBufferView& operator=(const IBufferView&) = delete;

      protected:
        IBufferView() = default;
        IBufferView(IBufferView&&) noexcept = default;
        IBufferView& operator=(IBufferView&&) noexcept = default;
    };
}  // namespace aetherion
