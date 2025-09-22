#pragma once

#include <memory>
#include <span>
#include <vector>

#include "aetherion/gpu/backend/render_definitions.hpp"

namespace aetherion {
    // Forward declarations
    class IDescriptorSetLayout;
    class IGPUImage;
    class ISampler;
    class IGPUBufferView;
    class IGPUImageView;
    class IGPUBuffer;
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
    class IGPUQueue;
    class IRenderSurface;
    class IWindow;
    struct CommandPoolDescription;
    struct CommandGPUBufferDescription;
    struct GPUBufferDescription;
    struct GPUBufferViewDescription;
    struct GPUImageDescription;
    struct GPUImageViewDescription;
    struct SamplerDescription;
    struct ShaderDescription;
    struct PipelineLayoutDescription;
    struct ComputePipelineDescription;
    struct GraphicsPipelineDescription;
    struct DescriptorSetLayoutDescription;
    struct DescriptorSetDescription;
    struct DescriptorPoolDescription;
    struct PushConstantRangeDescription;
    struct GPUQueueDescription;
    struct GPUFenceDescription;
    struct GPUBinarySemaphoreDescription;
    struct GPUTimelineSemaphoreDescription;
    struct SwapchainDescription;

    struct GPUQueueFamilyProperties {
        GPUQueueTypeFlags queueFlags;
        uint32_t queueCount;
    };

    struct PhysicalGPUDeviceDescription {
        /*GPUQueueTypeFlags requiredQueueTypes;
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

    struct GPUQueueFamilyDescription {
        uint32_t queueFamilyIndex;
        std::vector<float> queuePriorities;
    };

    struct GPUQueueSelectionInformation {
        std::vector<float> priorities;
    };

    struct GPUDeviceDescription {
        class IGPUPhysicalDevice* physicalDevice;
        std::vector<GPUQueueFamilyDescription> queueFamilyDescriptions;
    };

    struct DescriptorWriteDescriptorGPUImageDescription {
        IGPUImageView* imageView{};
        ISampler* sampler{};
        GPUImageLayout imageLayout = GPUImageLayout::Undefined;
    };

    struct DescriptorWriteDescriptorGPUBufferDescription {
        IGPUBuffer* buffer{};
        size_t offset = 0;
        size_t range = WHOLE_BUFFER_SIZE;
    };

    struct DescriptorWriteDescriptorTexelBufferViewDescription {
        IGPUBufferView* bufferView;
    };

    struct DescriptorWriteDescription {
        uint32_t dstBinding;
        uint32_t dstArrayElement;
        IDescriptorSet* dstSet;
        DescriptorType descriptorType;
        std::vector<DescriptorWriteDescriptorGPUImageDescription> images;
        std::vector<DescriptorWriteDescriptorGPUBufferDescription> buffers;
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

    class IGPUPhysicalDevice {
      public:
        virtual ~IGPUPhysicalDevice() = 0;

        IGPUPhysicalDevice(const IGPUPhysicalDevice&) = delete;
        IGPUPhysicalDevice& operator=(const IGPUPhysicalDevice&) = delete;

        virtual const GPUQueueFamilyProperties& getGPUQueueFamilyProperties(
            uint32_t familyIndex) const
            = 0;

      protected:
        IGPUPhysicalDevice() = default;
        IGPUPhysicalDevice(IGPUPhysicalDevice&&) noexcept = default;
        IGPUPhysicalDevice& operator=(IGPUPhysicalDevice&&) noexcept = default;
    };

    class IGPUDevice {
      public:
        virtual ~IGPUDevice() = 0;

        IGPUDevice(const IGPUDevice&) = delete;
        IGPUDevice& operator=(const IGPUDevice&) = delete;

        virtual void waitIdle() = 0;

        virtual std::unique_ptr<ICommandPool> createCommandPool(
            const CommandPoolDescription& description)
            = 0;

        virtual std::unique_ptr<ICommandBuffer> allocateCommandBuffer(
            ICommandPool& pool, const CommandGPUBufferDescription& description)
            = 0;
        virtual std::vector<std::unique_ptr<ICommandBuffer>> allocateCommandBuffers(
            ICommandPool& pool, uint32_t count, const CommandGPUBufferDescription& description)
            = 0;

        virtual void freeCommandBuffers(
            ICommandPool& pool, std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers)
            = 0;

        virtual std::unique_ptr<IGPUBuffer> createBuffer(const GPUBufferDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUBufferView> createBufferView(
            const GPUBufferViewDescription& description)
            = 0;

        virtual std::unique_ptr<IGPUImage> createImage(const GPUImageDescription& description) = 0;

        virtual std::unique_ptr<IGPUImageView> createImageView(
            const GPUImageViewDescription& description)
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

        virtual std::unique_ptr<IGPUQueue> getQueue(const GPUQueueDescription& description) = 0;

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
        IGPUDevice() = default;
        IGPUDevice(IGPUDevice&&) noexcept = default;
        IGPUDevice& operator=(IGPUDevice&&) noexcept = default;
    };
}  // namespace aetherion
