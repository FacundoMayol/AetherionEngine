#pragma once

#include <memory>
#include <span>
#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"

namespace aetherion {
    // Forward declarations
    class IDescriptorSetLayout;
    class IImage;
    class ISampler;
    class IBufferView;
    class IImageView;
    class IBuffer;
    class IDescriptorPool;
    class IDescriptorSet;
    class IPushConstantRange;
    class ICommandPool;
    class ICommandBuffer;
    class IPipelineLayout;
    class IPipeline;
    class IShader;
    class ISwapchain;
    class IGPUFence;
    class IGPUBinarySemaphore;
    class IGPUTimelineSemaphore;
    class IQueue;
    class ISurface;
    class IWindow;
    struct CommandPoolDescription;
    struct CommandBufferDescription;
    struct BufferDescription;
    struct BufferViewDescription;
    struct ImageDescription;
    struct ImageViewDescription;
    struct SamplerDescription;
    struct ShaderDescription;
    struct PipelineLayoutDescription;
    struct ComputePipelineDescription;
    struct GraphicsPipelineDescription;
    struct DescriptorSetLayoutDescription;
    struct DescriptorSetDescription;
    struct DescriptorPoolDescription;
    struct PushConstantRangeDescription;
    struct QueueDescription;
    struct GPUFenceDescription;
    struct GPUBinarySemaphoreDescription;
    struct GPUTimelineSemaphoreDescription;
    struct SwapchainDescription;

    struct QueueFamilyProperties {
        RenderQueueTypeFlags queueFlags;
        uint32_t queueCount;
    };

    struct PhysicalDeviceDescription {
        /*RenderQueueTypeFlags requiredQueueTypes;
        std::vector<const char*> requiredDeviceExtensions;
        std::vector<const char*> optionalDeviceExtensions;
        bool enableSwapchainSupport = true;
        bool enableAnisotropicFiltering = false;
        bool enableShaderInt16 = false;
        bool enableShaderInt64 = false;
        bool enableGeometryShader = false;
        bool enableTessellationShader = false;
        bool enableWideLines = false;
        bool enableMultiViewport = false;*/
        IWindow* primaryWindow;
    };

    struct QueueFamilyDescription {
        uint32_t queueFamilyIndex;
        std::vector<float> queuePriorities;
    };

    struct QueueSelectionInformation {
        std::vector<float> priorities;
    };

    struct DeviceDescription {
        class IPhysicalDevice* physicalDevice;
        std::vector<QueueFamilyDescription> queueFamilyDescriptions;
    };

    struct DescriptorWriteDescriptorImageDescription {
        IImageView* imageView;
        ISampler* sampler;
        ImageLayout imageLayout = ImageLayout::Undefined;
    };

    struct DescriptorWriteDescriptorBufferDescription {
        IBuffer* buffer;
        size_t offset = 0;
        size_t range = WHOLE_BUFFER_SIZE;
    };

    struct DescriptorWriteDescriptorTexelBufferViewDescription {
        IBufferView* bufferView;
    };

    struct DescriptorWriteDescription {
        uint32_t dstBinding;
        uint32_t dstArrayElement;
        IDescriptorSet* dstSet;
        DescriptorType descriptorType;
        std::vector<DescriptorWriteDescriptorImageDescription> images;
        std::vector<DescriptorWriteDescriptorBufferDescription> buffers;
        std::vector<DescriptorWriteDescriptorTexelBufferViewDescription> texelBuffers;
    };

    struct DescriptorCopyDescription {
        uint32_t srcBinding;
        uint32_t srcArrayElement;
        IDescriptorSet* srcSet;
        uint32_t dstBinding;
        uint32_t dstArrayElement;
        IDescriptorSet* dstSet;
        uint32_t descriptorCount;
    };

    class IPhysicalDevice {
      public:
        virtual ~IPhysicalDevice() = 0;

        IPhysicalDevice(const IPhysicalDevice&) = delete;
        IPhysicalDevice& operator=(const IPhysicalDevice&) = delete;

        virtual const QueueFamilyProperties& getQueueFamilyProperties(uint32_t familyIndex) const
            = 0;

      protected:
        IPhysicalDevice() = default;
        IPhysicalDevice(IPhysicalDevice&&) noexcept = default;
        IPhysicalDevice& operator=(IPhysicalDevice&&) noexcept = default;
    };

    class IRenderDevice {
      public:
        virtual ~IRenderDevice() = 0;

        IRenderDevice(const IRenderDevice&) = delete;
        IRenderDevice& operator=(const IRenderDevice&) = delete;

        virtual void waitIdle() = 0;

        virtual std::unique_ptr<ICommandPool> createCommandPool(
            const CommandPoolDescription& description)
            = 0;

        virtual std::unique_ptr<ICommandBuffer> allocateCommandBuffer(
            ICommandPool& pool, const CommandBufferDescription& description)
            = 0;
        virtual std::vector<std::unique_ptr<ICommandBuffer>> allocateCommandBuffers(
            ICommandPool& pool, uint32_t count, const CommandBufferDescription& description)
            = 0;

        virtual void freeCommandBuffers(
            ICommandPool& pool, std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers)
            = 0;

        virtual std::unique_ptr<IBuffer> createBuffer(const BufferDescription& description) = 0;

        virtual std::unique_ptr<IBufferView> createBufferView(
            const BufferViewDescription& description)
            = 0;

        virtual std::unique_ptr<IImage> createImage(const ImageDescription& description) = 0;

        virtual std::unique_ptr<IImageView> createImageView(const ImageViewDescription& description)
            = 0;

        virtual std::unique_ptr<ISampler> createSampler(const SamplerDescription& description) = 0;

        virtual std::unique_ptr<IShader> createShader(const ShaderDescription& description) = 0;

        virtual std::unique_ptr<IPipelineLayout> createPipelineLayout(
            const PipelineLayoutDescription& description)
            = 0;

        virtual std::unique_ptr<IPipeline> createComputePipeline(
            const ComputePipelineDescription& description)
            = 0;

        virtual std::unique_ptr<IPipeline> createGraphicsPipeline(
            const GraphicsPipelineDescription& description)
            = 0;

        virtual std::unique_ptr<IDescriptorSetLayout> createDescriptorSetLayout(
            const DescriptorSetLayoutDescription& description)
            = 0;

        virtual std::unique_ptr<IDescriptorPool> createDescriptorPool(
            const DescriptorPoolDescription& description)
            = 0;

        virtual std::unique_ptr<IPushConstantRange> createPushConstantRange(
            const PushConstantRangeDescription& description)
            = 0;

        virtual std::unique_ptr<ISwapchain> createSwapchain(const SwapchainDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUFence> createGPUFence(const GPUFenceDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUBinarySemaphore> createGPUBinarySemaphore(
            const GPUBinarySemaphoreDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUTimelineSemaphore> createGPUTimelineSemaphore(
            const GPUTimelineSemaphoreDescription& description)
            = 0;

        virtual std::unique_ptr<IQueue> getQueue(const QueueDescription& description) = 0;

        virtual std::unique_ptr<IDescriptorSet> allocateDescriptorSet(
            IDescriptorPool& pool, const DescriptorSetDescription& description)
            = 0;

        virtual std::vector<std::unique_ptr<IDescriptorSet>> allocateDescriptorSets(
            IDescriptorPool& pool, std::span<const DescriptorSetDescription> descriptions)
            = 0;

        virtual void freeDescriptorSets(
            IDescriptorPool& pool, std::span<std::reference_wrapper<IDescriptorSet>> descriptorSets)
            = 0;

        virtual void updateDescriptorSets(
            std::span<const DescriptorWriteDescription> descriptorWrites,
            std::span<const DescriptorCopyDescription> descriptorCopies)
            = 0;

      protected:
        IRenderDevice() = default;
        IRenderDevice(IRenderDevice&&) noexcept = default;
        IRenderDevice& operator=(IRenderDevice&&) noexcept = default;
    };
}  // namespace aetherion
