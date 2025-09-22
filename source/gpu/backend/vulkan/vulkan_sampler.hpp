#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/gpu/backend/sampler.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanSampler : public ISampler {
      public:
        VulkanSampler() = delete;
        VulkanSampler(VulkanDevice& device, const SamplerDescription& description);
        VulkanSampler(vk::Device device, vk::Sampler sampler);
        ~VulkanSampler() noexcept override;

        VulkanSampler(const VulkanSampler&) = delete;
        VulkanSampler& operator=(const VulkanSampler&) = delete;

        VulkanSampler(VulkanSampler&&) noexcept;
        VulkanSampler& operator=(VulkanSampler&&) noexcept;

        inline vk::Sampler getVkSampler() const { return sampler_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;

        vk::Sampler sampler_;
    };
}  // namespace aetherion