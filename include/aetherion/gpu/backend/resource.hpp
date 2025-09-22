#pragma once

namespace aetherion {
    class IGPUResource {
      public:
        virtual ~IGPUResource() = 0;

        IGPUResource(const IGPUResource&) = delete;
        IGPUResource& operator=(const IGPUResource&) = delete;

      protected:
        IGPUResource() = default;
        IGPUResource(IGPUResource&&) noexcept = default;
        IGPUResource& operator=(IGPUResource&&) noexcept = default;
    };
}  // namespace aetherion
