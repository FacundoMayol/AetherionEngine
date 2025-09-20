#pragma once

#include <memory>
#include <optional>
#include <span>
#include <vector>

#include "aetherion/graphics/backend/image.hpp"
#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/util/common_definitions.hpp"

namespace aetherion {
    // Forward declarations
    class IImage;
    class IImageView;
    class IBuffer;
    class IPipeline;
    class IPipelineLayout;
    class IDescriptorSet;
    class IPushConstantRange;

    struct CommandPoolDescription {
        uint32_t queueFamilyIndex;
        CommandPoolBehaviorFlags flags;
    };

    struct BufferCopyRegion {
        size_t srcOffset;
        size_t dstOffset;
        size_t size;
    };

    struct VertexBufferBindingDescription {
        IBuffer* buffer;
        size_t offset;
        size_t size;
        size_t stride;
    };

    struct AttachmentDescription {
        IImage* image;
        IImageView* imageView;
        ImageLayout imageLayout;
        ResolveMode resolveMode = ResolveMode::None;
        IImageView* resolveImageView;  // Optional, used if resolveMode is not None.
        ImageLayout resolveImageLayout = ImageLayout::Undefined;
        AttachmentLoadOp loadOp = AttachmentLoadOp::Clear;
        AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
        ClearValue clearValue = ClearValue(
            ColorClearValue{std::in_place_index<0>, std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}});
    };

    struct RenderDescription {
        Rect2Diu renderArea;
        uint32_t layerCount;
        uint32_t viewMask = 0;
        std::vector<AttachmentDescription> colorAttachments;
        std::optional<AttachmentDescription> depthAttachment;
        std::optional<AttachmentDescription> stencilAttachment;
    };

    struct GeneralMemoryBarrierDescription {
        PipelineStageFlags srcStageFlags = {};
        AccessTypeFlags srcAccessFlags = {};
        PipelineStageFlags dstStageFlags = {};
        AccessTypeFlags dstAccessFlags = {};
    };

    struct BufferBarrierDescription {
        IBuffer* buffer;
        PipelineStageFlags srcStageFlags = {};
        AccessTypeFlags srcAccessFlags = {};
        PipelineStageFlags dstStageFlags = {};
        AccessTypeFlags dstAccessFlags = {};
        uint32_t srcQueueFamilyIndex = 0;
        uint32_t dstQueueFamilyIndex = 0;
        uint32_t offset = 0;
        uint32_t size = 0;  // NOTE: size = 0 means whole buffer.
    };

    struct ImageBarrierDescription {
        IImage* image;
        ImageLayout oldLayout = ImageLayout::Undefined;
        ImageLayout newLayout = ImageLayout::Undefined;
        PipelineStageFlags srcStageFlags = {};
        AccessTypeFlags srcAccessFlags = {};
        PipelineStageFlags dstStageFlags = {};
        AccessTypeFlags dstAccessFlags = {};
        uint32_t srcQueueFamilyIndex = 0;
        uint32_t dstQueueFamilyIndex = 0;
        ImageSubresourceDescription subresource = {};
    };

    struct CommandBufferDescription {
        CommandBufferLevel level = CommandBufferLevel::Primary;
    };

    class ICommandBuffer {
      public:
        virtual ~ICommandBuffer() = 0;

        ICommandBuffer(const ICommandBuffer&) = delete;
        ICommandBuffer& operator=(const ICommandBuffer&) = delete;

        virtual void begin(CommandBufferUsageFlags flags = CommandBufferUsage::None) = 0;
        virtual void reset(bool releaseResources = false) = 0;
        virtual void end() = 0;

        virtual void beginRendering(const RenderDescription& renderDescription) = 0;
        virtual void endRendering() = 0;

        virtual void executeCommands(
            std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers)
            = 0;

        virtual void draw(uint32_t vertexCount, uint32_t instanceCount = 1,
                          uint32_t firstVertex = 0, uint32_t firstInstance = 0)
            = 0;
        virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1,
                                 uint32_t firstIndex = 0, uint32_t baseVertex = 0,
                                 uint32_t firstInstance = 0)
            = 0;
        virtual void dispatchCompute(uint32_t x, uint32_t y, uint32_t z) = 0;

        virtual void setViewport(Rect2Df viewport, float minDepth = 0.0f, float maxDepth = 1.0f)
            = 0;
        // TODO: Handle multiple scissors.
        virtual void setScissor(Rect2Di scissor) = 0;

        virtual void clear(IImage& image, ImageLayout layout,
                           const std::vector<ImageRangeDescription>& ranges,
                           const ClearValue& clearValue = ClearValue(ColorClearValue{
                               std::in_place_index<0>,
                               std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}))
            = 0;

        virtual void bindPipeline(IPipeline& pipeline) = 0;

        virtual void bindDescriptorSets(
            IPipelineLayout& pipelineLayout, PipelineBindPoint bindPoint, uint32_t firstSet,
            std::span<std::reference_wrapper<IDescriptorSet>> descriptorSets)
            = 0;
        virtual void pushConstantRange(IPipelineLayout& pipelineLayout,
                                       IPushConstantRange& pushConstantRange,
                                       std::span<const std::byte> data)
            = 0;

        virtual void bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount,
                                       std::span<const VertexBufferBindingDescription> bindings)
            = 0;
        virtual void bindIndexBuffer(IBuffer& buffer, size_t offset, IndexType indexType) = 0;

        virtual void copyBuffer(IBuffer& src, IBuffer& dst,
                                const std::vector<BufferCopyRegion>& regions)
            = 0;

        virtual void barrier(std::span<const GeneralMemoryBarrierDescription> generalBarriers,
                             std::span<const BufferBarrierDescription> bufferBarriers,
                             std::span<const ImageBarrierDescription> imageBarriers)
            = 0;

      protected:
        ICommandBuffer() = default;
        ICommandBuffer(ICommandBuffer&&) noexcept = default;
        ICommandBuffer& operator=(ICommandBuffer&&) noexcept = default;
    };

    class ICommandPool {
      public:
        virtual ~ICommandPool() = 0;

        ICommandPool(const ICommandPool&) = delete;
        ICommandPool& operator=(const ICommandPool&) = delete;

        virtual std::unique_ptr<ICommandBuffer> allocateCommandBuffer(
            const CommandBufferDescription& description)
            = 0;
        virtual std::vector<std::unique_ptr<ICommandBuffer>> allocateCommandBuffers(
            uint32_t count, const CommandBufferDescription& description)
            = 0;

        virtual void reset(bool releaseResources = false) = 0;

        virtual void freeCommandBuffers(
            std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers)
            = 0;

      protected:
        ICommandPool() = default;
        ICommandPool(ICommandPool&&) noexcept = default;
        ICommandPool& operator=(ICommandPool&&) noexcept = default;
    };
}  // namespace aetherion
