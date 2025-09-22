#include "vulkan_shader.hpp"

#include "vulkan_device.hpp"

namespace aetherion {
    VulkanShader::VulkanShader(VulkanDevice& device, const ShaderDescription& description)
        : device_(device.getVkDevice()) {
        assert(description.code.size() % sizeof(uint32_t) == 0
               && "Shader code size must be a multiple of 4 bytes");
        auto shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
                                          .setCodeSize(description.code.size())
                                          .setPCode(static_cast<const uint32_t*>(
                                              static_cast<const void*>(description.code.data())));

        shaderModule_ = device_.createShaderModule(shaderModuleCreateInfo);
    }

    VulkanShader::VulkanShader(vk::Device device, vk::ShaderModule shaderModule)
        : device_(device), shaderModule_(shaderModule) {}

    VulkanShader::~VulkanShader() noexcept { clear(); }

    VulkanShader::VulkanShader(VulkanShader&& other) noexcept
        : IShader(std::move(other)), device_(other.device_), shaderModule_(other.shaderModule_) {
        other.device_ = nullptr;
        other.shaderModule_ = nullptr;
    }

    VulkanShader& VulkanShader::operator=(VulkanShader&& other) noexcept {
        if (this != &other) {
            clear();

            IShader::operator=(std::move(other));
            device_ = other.device_;
            shaderModule_ = other.shaderModule_;

            other.release();
        }
        return *this;
    }

    void VulkanShader::clear() noexcept {
        if (shaderModule_ && device_) {
            device_.destroyShaderModule(shaderModule_);
            shaderModule_ = nullptr;
        }
        device_ = nullptr;
    }

    void VulkanShader::release() noexcept {
        shaderModule_ = nullptr;
        device_ = nullptr;
    }
}  // namespace aetherion