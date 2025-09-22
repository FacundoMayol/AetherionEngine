#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/command_buffer.hpp"

namespace aetherion {
    // Forward declarations
    class IRenderDevice;
    class IDescriptorPool;
    class VulkanDevice;
    class VulkanCommandPool;

    class VulkanCommandBuffer : public ICommandBuffer {
      public:
        static std::vector<std::unique_ptr<ICommandBuffer>> allocateCommandBuffers(
            IRenderDevice& device, ICommandPool& commandPool, uint32_t count,
            const CommandBufferDescription& description);
        static std::vector<std::unique_ptr<ICommandBuffer>> allocateCommandBuffers(
            VulkanDevice& device, VulkanCommandPool& commandPool, uint32_t count,
            const CommandBufferDescription& description);
        static std::vector<std::unique_ptr<ICommandBuffer>> allocateCommandBuffers(
            vk::Device device, vk::CommandPool commandPool, uint32_t count,
            const CommandBufferDescription& description);

        VulkanCommandBuffer() = delete;
        VulkanCommandBuffer(VulkanDevice& device, VulkanCommandPool& commandPool,
                            const CommandBufferDescription& description);
        VulkanCommandBuffer(vk::Device device, vk::CommandPool commandPool,
                            vk::CommandBuffer commandBuffer, bool shouldFree = false);
        ~VulkanCommandBuffer() noexcept override;

        VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
        VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;

        VulkanCommandBuffer(VulkanCommandBuffer&&) noexcept;
        VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) noexcept;

        static void freeCommandBuffers(
            IRenderDevice& device, ICommandPool& commandPool,
            std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers);
        static void freeCommandBuffers(
            VulkanDevice& device, VulkanCommandPool& commandPool,
            std::span<std::reference_wrapper<VulkanCommandBuffer>> commandBuffers);
        static void freeCommandBuffers(vk::Device device, vk::CommandPool commandPool,
                                       std::span<vk::CommandBuffer> commandBuffers);

        void begin(CommandBufferUsageFlags flags = CommandBufferUsage::None) override;
        void reset(bool releaseResources = false) override;
        void end() override;

        void beginRendering(const RenderDescription& renderDescription) override;
        void endRendering() override;

        void executeCommands(
            std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers) override;

        void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0,
                  uint32_t firstInstance = 0) override;
        void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0,
                         uint32_t baseVertex = 0, uint32_t firstInstance = 0) override;
        void dispatchCompute(uint32_t x, uint32_t y, uint32_t z) override;

        void setViewport(Rect2Df viewport, float minDepth = 0.0f, float maxDepth = 1.0f) override;
        void setScissor(Rect2Di scissor) override;

        void clear(IRenderImage& image, RenderImageLayout layout,
                   const std::vector<RenderImageRangeDescription>& ranges,
                   const ClearValue& clearValue = ClearValue(ColorClearValue{
                       std::in_place_index<0>,
                       std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}})) override;

        void bindPipeline(IPipeline& pipeline) override;

        void bindDescriptorSets(
            IPipelineLayout& pipelineLayout, PipelineBindPoint bindPoint, uint32_t firstSet,
            std::span<std::reference_wrapper<IDescriptorSet>> descriptorSets) override;
        void pushConstantRange(IPipelineLayout& pipelineLayout,
                               IPushConstantRange& pushConstantRange,
                               std::span<const std::byte> data) override;

        void bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount,
                               std::span<const VertexBufferBindingDescription> bindings) override;
        void bindIndexBuffer(IRenderBuffer& buffer, size_t offset, IndexType indexType) override;

        void copyBuffer(IRenderBuffer& src, IRenderBuffer& dst,
                        const std::vector<BufferCopyRegion>& regions) override;

        void barrier(std::span<const GeneralMemoryBarrierDescription> generalBarriers,
                     std::span<const BufferBarrierDescription> bufferBarriers,
                     std::span<const ImageBarrierDescription> imageBarriers) override;

        inline vk::CommandBuffer getVkCommandBuffer() const { return commandBuffer_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;
        vk::CommandPool commandPool_;

        vk::CommandBuffer commandBuffer_;

        bool shouldFreeCommandBuffer_;
    };

    class VulkanCommandPool : public ICommandPool {
      public:
        VulkanCommandPool() = delete;
        VulkanCommandPool(VulkanDevice& device, const CommandPoolDescription& description);
        VulkanCommandPool(vk::Device device, vk::CommandPool commandPool,
                          bool freeCommandBufferSupport);
        ~VulkanCommandPool() noexcept override;

        VulkanCommandPool(const VulkanCommandPool&) = delete;
        VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;

        VulkanCommandPool(VulkanCommandPool&&) noexcept;
        VulkanCommandPool& operator=(VulkanCommandPool&&) noexcept;

        void reset(bool releaseResources = false) override;

        inline vk::CommandPool getVkCommandPool() const { return commandPool_; }

        inline bool supportsFreeCommandBuffer() const { return freeCommandBufferSupport_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;

        vk::CommandPool commandPool_;

        bool freeCommandBufferSupport_ = false;
    };
}  // namespace aetherion