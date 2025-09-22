#pragma once

#include <VkBootstrap.h>

#include <unordered_map>
#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

#include "aetherion/gpu/backend/device.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDriver;

    class VulkanGPUPhysicalDevice : public IGPUPhysicalDevice {
      public:
        VulkanGPUPhysicalDevice() = delete;
        VulkanGPUPhysicalDevice(VulkanDriver& driver,
                                const PhysicalGPUDeviceDescription& description);
        VulkanGPUPhysicalDevice(vk::Instance instance,
                                vkb::GPUPhysicalDevice builderGPUPhysicalDevice,
                                vk::GPUPhysicalDevice physicalDevice);
        ~VulkanGPUPhysicalDevice() noexcept override;

        VulkanGPUPhysicalDevice(const VulkanGPUPhysicalDevice&) = delete;
        VulkanGPUPhysicalDevice& operator=(const VulkanGPUPhysicalDevice&) = delete;

        VulkanGPUPhysicalDevice(VulkanGPUPhysicalDevice&&) noexcept;
        VulkanGPUPhysicalDevice& operator=(VulkanGPUPhysicalDevice&&) noexcept;

        const GPUQueueFamilyProperties& getGPUQueueFamilyProperties(
            uint32_t familyIndex) const override;

        inline vkb::GPUPhysicalDevice getVkBuilderGPUPhysicalDevice() const {
            return builderGPUPhysicalDevice_;
        }
        inline vk::GPUPhysicalDevice getVkGPUPhysicalDevice() const { return physicalDevice_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Instance instance_;

        vkb::GPUPhysicalDevice builderGPUPhysicalDevice_;
        vk::GPUPhysicalDevice physicalDevice_;

        std::unordered_map<uint32_t, GPUQueueFamilyProperties> queueFamilyProperties_;
    };

    class VulkanDevice : public IGPUDevice {
      public:
        VulkanDevice() = delete;
        VulkanDevice(VulkanDriver& driver, const GPUDeviceDescription& description);
        VulkanDevice(vk::Instance instance, vk::GPUPhysicalDevice physicalDevice,
                     vkb::Device builderDevice, vk::Device device, vma::Allocator allocator);
        ~VulkanDevice() noexcept override;

        VulkanDevice(const VulkanDevice&) = delete;
        VulkanDevice& operator=(const VulkanDevice&) = delete;
        VulkanDevice(VulkanDevice&&) noexcept;
        VulkanDevice& operator=(VulkanDevice&&) noexcept;

        void waitIdle() override;

        std::unique_ptr<ICommandPool> createCommandPool(
            const CommandPoolDescription& description) override;

        std::unique_ptr<ICommandBuffer> allocateCommandBuffer(
            ICommandPool& pool, const CommandGPUBufferDescription& description) override;
        std::vector<std::unique_ptr<ICommandBuffer>> allocateCommandBuffers(
            ICommandPool& pool, uint32_t count,
            const CommandGPUBufferDescription& description) override;

        void freeCommandBuffers(
            ICommandPool& pool,
            std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers) override;

        std::unique_ptr<IGPUBuffer> createBuffer(const GPUBufferDescription& description) override;

        std::unique_ptr<IGPUBufferView> createBufferView(
            const GPUBufferViewDescription& description) override;

        std::unique_ptr<IGPUImage> createImage(const GPUImageDescription& description) override;

        std::unique_ptr<IGPUImageView> createImageView(
            const GPUImageViewDescription& description) override;

        std::unique_ptr<ISampler> createSampler(const SamplerDescription& description) override;

        std::unique_ptr<IShader> createShader(const ShaderDescription& description) override;

        std::unique_ptr<IPipelineLayout> createPipelineLayout(
            const PipelineLayoutDescription& description) override;

        std::unique_ptr<IPipeline> createComputePipeline(
            const ComputePipelineDescription& description) override;

        std::unique_ptr<IPipeline> createGraphicsPipeline(
            const GraphicsPipelineDescription& description) override;

        std::unique_ptr<IDescriptorSetLayout> createDescriptorSetLayout(
            const DescriptorSetLayoutDescription& description) override;

        std::unique_ptr<IDescriptorPool> createDescriptorPool(
            const DescriptorPoolDescription& description) override;

        std::unique_ptr<IPushConstantRange> createPushConstantRange(
            const PushConstantRangeDescription& description) override;

        std::unique_ptr<ISwapchain> createSwapchain(
            const SwapchainDescription& description) override;

        std::unique_ptr<IGPUFence> createGPUFence(const GPUFenceDescription& description) override;

        std::unique_ptr<IGPUBinarySemaphore> createGPUBinarySemaphore(
            const GPUBinarySemaphoreDescription& description) override;

        std::unique_ptr<IGPUTimelineSemaphore> createGPUTimelineSemaphore(
            const GPUTimelineSemaphoreDescription& description) override;

        std::unique_ptr<IGPUQueue> getQueue(const GPUQueueDescription& description) override;

        std::unique_ptr<IDescriptorSet> allocateDescriptorSet(
            IDescriptorPool& pool, const DescriptorSetDescription& description) override;

        std::vector<std::unique_ptr<IDescriptorSet>> allocateDescriptorSets(
            IDescriptorPool& pool, std::span<const DescriptorSetDescription> descriptions) override;

        void freeDescriptorSets(
            IDescriptorPool& pool,
            std::span<std::reference_wrapper<IDescriptorSet>> descriptorSets) override;

        void updateDescriptorSets(
            std::span<const DescriptorWriteDescription> descriptorWrites,
            std::span<const DescriptorCopyDescription> descriptorCopies) override;

        inline vkb::Device getVkBuilderDevice() const { return builderDevice_; }
        inline vk::Device getVkDevice() const { return device_; }

        inline vma::Allocator getVmaAllocator() const { return allocator_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Instance instance_;
        vk::GPUPhysicalDevice physicalDevice_;

        vma::Allocator allocator_;

        vkb::Device builderDevice_;
        vk::Device device_;
    };
}  // namespace aetherion
