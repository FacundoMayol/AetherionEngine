#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/shader.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanShader : public IShader {
      public:
        VulkanShader() = delete;
        VulkanShader(VulkanDevice& device, const ShaderDescription& description);
        VulkanShader(VulkanDevice& device, vk::ShaderModule shaderModule);
        virtual ~VulkanShader() noexcept override;

        VulkanShader(const VulkanShader&) = delete;
        VulkanShader& operator=(const VulkanShader&) = delete;

        VulkanShader(VulkanShader&&) noexcept;
        VulkanShader& operator=(VulkanShader&&) noexcept;

        inline vk::ShaderModule getVkShaderModule() const { return shaderModule_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;

        vk::ShaderModule shaderModule_;
    };
}  // namespace aetherion