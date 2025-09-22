#pragma once

#include <span>

#include "aetherion/gpu/backend/resource.hpp"

namespace aetherion {
    struct ShaderDescription {
        std::span<const std::byte> code;
    };

    class IShader : public IGPUResource {
      public:
        ~IShader() override = 0;

        IShader(const IShader&) = delete;
        IShader& operator=(const IShader&) = delete;

      protected:
        IShader() = default;
        IShader(IShader&&) noexcept = default;
        IShader& operator=(IShader&&) noexcept = default;
    };
}  // namespace aetherion
