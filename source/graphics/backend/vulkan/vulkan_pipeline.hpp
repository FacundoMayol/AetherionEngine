#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/pipeline.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanPipelineLayout : public IPipelineLayout {
      public:
        VulkanPipelineLayout() = delete;
        VulkanPipelineLayout(VulkanDevice& device, const PipelineLayoutDescription& description);
        VulkanPipelineLayout(VulkanDevice& device, vk::PipelineLayout pipelineLayout);
        virtual ~VulkanPipelineLayout() noexcept override;

        VulkanPipelineLayout(const VulkanPipelineLayout&) = delete;
        VulkanPipelineLayout& operator=(const VulkanPipelineLayout&) = delete;

        VulkanPipelineLayout(VulkanPipelineLayout&&) noexcept;
        VulkanPipelineLayout& operator=(VulkanPipelineLayout&&) noexcept;

        inline vk::PipelineLayout getVkPipelineLayout() const { return pipelineLayout_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;

        vk::PipelineLayout pipelineLayout_;
    };

    class VulkanPipeline : public IPipeline {
      public:
        VulkanPipeline() = delete;
        VulkanPipeline(VulkanDevice& device, const ComputePipelineDescription& description);
        VulkanPipeline(VulkanDevice& device, const GraphicsPipelineDescription& description);
        VulkanPipeline(VulkanDevice& device, vk::Pipeline pipeline, PipelineBindPoint pipelineType);
        virtual ~VulkanPipeline() noexcept override;

        VulkanPipeline(const VulkanPipeline&) = delete;
        VulkanPipeline& operator=(const VulkanPipeline&) = delete;

        VulkanPipeline(VulkanPipeline&&) noexcept;
        VulkanPipeline& operator=(VulkanPipeline&&) noexcept;

        inline virtual PipelineBindPoint getPipelineType() const override { return pipelineType_; }

        inline vk::Pipeline getVkPipeline() const { return pipeline_; }

        void clear() noexcept;
        void release() noexcept;

      protected:
        VulkanDevice* device_;

        vk::Pipeline pipeline_;

        PipelineBindPoint pipelineType_;
    };
}  // namespace aetherion