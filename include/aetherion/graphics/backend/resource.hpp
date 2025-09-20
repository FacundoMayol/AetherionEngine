#pragma once

namespace aetherion {
    class IResource {
      public:
        virtual ~IResource() = 0;

        IResource(const IResource&) = delete;
        IResource& operator=(const IResource&) = delete;

      protected:
        IResource() = default;
        IResource(IResource&&) noexcept = default;
        IResource& operator=(IResource&&) noexcept = default;
    };
}  // namespace aetherion
