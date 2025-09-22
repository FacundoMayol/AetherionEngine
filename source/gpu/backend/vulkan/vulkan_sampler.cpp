#include "vulkan_sampler.hpp"

#include "vulkan_device.hpp"
#include "vulkan_render_definitions.hpp"

namespace aetherion {
    VulkanSampler::VulkanSampler(VulkanDevice& device, const SamplerDescription& description)
        : device_(device.getVkDevice()) {
        sampler_ = device_.createSampler(
            vk::SamplerCreateInfo()
                .setMagFilter(toVkFilter(description.magFilter))
                .setMinFilter(toVkFilter(description.minFilter))
                .setMipmapMode(toVkSamplerMipmapMode(description.mipmapMode))
                .setAddressModeU(toVkSamplerAddressMode(description.addressModeU))
                .setAddressModeV(toVkSamplerAddressMode(description.addressModeV))
                .setAddressModeW(toVkSamplerAddressMode(description.addressModeW))
                .setMipLodBias(description.mipLodBias)
                .setAnisotropyEnable(description.enableAnisotropicFiltering)
                .setMaxAnisotropy(description.maxAnisotropy)
                .setCompareEnable(description.enableCompare)
                .setCompareOp(toVkCompareOp(description.compareOp))
                .setMinLod(description.minLod)
                .setMaxLod(description.maxLod)
                .setBorderColor(toVkBorderColor(description.borderColor))
                .setUnnormalizedCoordinates(description.unnormalizedCoordinates));
    }

    VulkanSampler::VulkanSampler(vk::Device device, vk::Sampler sampler)
        : device_(device), sampler_(sampler) {}

    VulkanSampler::~VulkanSampler() noexcept { clear(); }

    VulkanSampler::VulkanSampler(VulkanSampler&& other) noexcept
        : ISampler(std::move(other)), device_(other.device_), sampler_(other.sampler_) {
        other.device_ = nullptr;
        other.sampler_ = nullptr;
    }

    VulkanSampler& VulkanSampler::operator=(VulkanSampler&& other) noexcept {
        if (this != &other) {
            clear();

            ISampler::operator=(std::move(other));
            device_ = other.device_;
            sampler_ = other.sampler_;

            other.release();
        }
        return *this;
    }

    void VulkanSampler::clear() noexcept {
        if (sampler_ && device_) {
            device_.destroySampler(sampler_);
            sampler_ = nullptr;
        }
        device_ = nullptr;
    }

    void VulkanSampler::release() noexcept {
        sampler_ = nullptr;
        device_ = nullptr;
    }
}  // namespace aetherion