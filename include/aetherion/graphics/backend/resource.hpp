#pragma once

namespace aetherion {
    class IRenderResource {
      public:
        virtual ~IRenderResource() = 0;

        IRenderResource(const IRenderResource&) = delete;
        IRenderResource& operator=(const IRenderResource&) = delete;

      protected:
        IRenderResource() = default;
        IRenderResource(IRenderResource&&) noexcept = default;
        IRenderResource& operator=(IRenderResource&&) noexcept = default;
    };
}  // namespace aetherion
