#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/sampler.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanSampler : public ISampler {
      public:
        VulkanSampler() = delete;
        VulkanSampler(VulkanDevice& device, const SamplerDescription& description);
        VulkanSampler(VulkanDevice& device, vk::Sampler sampler);
        virtual ~VulkanSampler() noexcept override;

        VulkanSampler(const VulkanSampler&) = delete;
        VulkanSampler& operator=(const VulkanSampler&) = delete;

        VulkanSampler(VulkanSampler&&) noexcept;
        VulkanSampler& operator=(VulkanSampler&&) noexcept;

        inline vk::Sampler getVkSampler() const { return sampler_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;

        vk::Sampler sampler_;
    };
}  // namespace aetherion