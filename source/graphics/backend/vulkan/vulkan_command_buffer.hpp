#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/command_buffer.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;
    class VulkanCommandPool;

    class VulkanCommandBuffer : public ICommandBuffer {
      public:
        static std::vector<std::unique_ptr<ICommandBuffer>> allocateCommandBuffers(
            VulkanDevice& device, VulkanCommandPool& commandPool, uint32_t count,
            const CommandBufferDescription& description);

        VulkanCommandBuffer() = delete;
        VulkanCommandBuffer(VulkanDevice& device, VulkanCommandPool& commandPool,
                            const CommandBufferDescription& description);
        VulkanCommandBuffer(VulkanDevice& device, VulkanCommandPool& commandPool,
                            vk::CommandBuffer commandBuffer);
        virtual ~VulkanCommandBuffer() noexcept override;

        VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
        VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;

        VulkanCommandBuffer(VulkanCommandBuffer&&) noexcept;
        VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) noexcept;

        static void freeCommandBuffers(
            VulkanDevice& device, VulkanCommandPool& commandPool,
            std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers);

        virtual void begin(CommandBufferUsageFlags flags = CommandBufferUsage::None) override;
        virtual void reset(bool releaseResources = false) override;
        virtual void end() override;

        virtual void beginRendering(const RenderDescription& renderDescription) override;
        virtual void endRendering() override;

        virtual void executeCommands(
            std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers) override;

        virtual void draw(uint32_t vertexCount, uint32_t instanceCount = 1,
                          uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;
        virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1,
                                 uint32_t firstIndex = 0, uint32_t baseVertex = 0,
                                 uint32_t firstInstance = 0) override;
        virtual void dispatchCompute(uint32_t x, uint32_t y, uint32_t z) override;

        virtual void setViewport(Rect2Df viewport, float minDepth = 0.0f,
                                 float maxDepth = 1.0f) override;
        virtual void setScissor(Rect2Di scissor) override;

        virtual void clear(IImage& image, ImageLayout layout,
                           const std::vector<ImageRangeDescription>& ranges,
                           const ClearValue& clearValue = ClearValue(ColorClearValue{
                               std::in_place_index<0>,
                               std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}})) override;

        virtual void bindPipeline(IPipeline& pipeline) override;

        virtual void bindDescriptorSets(
            IPipelineLayout& pipelineLayout, PipelineBindPoint bindPoint, uint32_t firstSet,
            std::span<std::reference_wrapper<IDescriptorSet>> descriptorSets) override;
        virtual void pushConstantRange(IPipelineLayout& pipelineLayout,
                                       IPushConstantRange& pushConstantRange,
                                       std::span<const std::byte> data) override;

        virtual void bindVertexBuffers(
            uint32_t firstBinding, uint32_t bindingCount,
            std::span<const VertexBufferBindingDescription> bindings) override;
        virtual void bindIndexBuffer(IBuffer& buffer, size_t offset, IndexType indexType) override;

        virtual void copyBuffer(IBuffer& src, IBuffer& dst,
                                const std::vector<BufferCopyRegion>& regions) override;

        virtual void barrier(std::span<const GeneralMemoryBarrierDescription> generalBarriers,
                             std::span<const BufferBarrierDescription> bufferBarriers,
                             std::span<const ImageBarrierDescription> imageBarriers) override;

        inline vk::CommandBuffer getVkCommandBuffer() const { return commandBuffer_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;
        VulkanCommandPool* commandPool_;

        vk::CommandBuffer commandBuffer_;
    };

    class VulkanCommandPool : public ICommandPool {
      public:
        VulkanCommandPool() = delete;
        VulkanCommandPool(VulkanDevice& device, const CommandPoolDescription& description);
        VulkanCommandPool(VulkanDevice& device, vk::CommandPool commandPool,
                          bool freeCommandBufferSupport);
        virtual ~VulkanCommandPool() noexcept override;

        VulkanCommandPool(const VulkanCommandPool&) = delete;
        VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;

        VulkanCommandPool(VulkanCommandPool&&) noexcept;
        VulkanCommandPool& operator=(VulkanCommandPool&&) noexcept;

        virtual std::unique_ptr<ICommandBuffer> allocateCommandBuffer(
            const CommandBufferDescription& description) override;
        virtual std::vector<std::unique_ptr<ICommandBuffer>> allocateCommandBuffers(
            uint32_t count, const CommandBufferDescription& description) override;

        virtual void reset(bool releaseResources = false) override;

        virtual void freeCommandBuffers(
            std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers) override;

        inline vk::CommandPool getVkCommandPool() const { return commandPool_; }

        inline bool supportsFreeCommandBuffer() const { return freeCommandBufferSupport_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;

        vk::CommandPool commandPool_;

        bool freeCommandBufferSupport_ = false;
    };
}  // namespace aetherion