#pragma once

#include "aetherion/gpu/backend/render_definitions.hpp"
#include "aetherion/gpu/backend/resource.hpp"

namespace aetherion {
    // Forward declarations
    class IGPUBuffer;

    struct GPUBufferViewDescription {
        IGPUBuffer* buffer{};
        Format format = Format::Undefined;  // NOTE: Ignored if buffer is not a texel buffer.
        uint32_t offset = 0;
        uint32_t range = 0;
    };

    class IGPUBufferView : public IGPUResource {
      public:
        ~IGPUBufferView() override = 0;

        IGPUBufferView(const IGPUBufferView&) = delete;
        IGPUBufferView& operator=(const IGPUBufferView&) = delete;

      protected:
        IGPUBufferView() = default;
        IGPUBufferView(IGPUBufferView&&) noexcept = default;
        IGPUBufferView& operator=(IGPUBufferView&&) noexcept = default;
    };
}  // namespace aetherion
