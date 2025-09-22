#include "vulkan_command_buffer.hpp"

#include "vulkan_buffer.hpp"
#include "vulkan_descriptor_set.hpp"
#include "vulkan_device.hpp"
#include "vulkan_image.hpp"
#include "vulkan_image_view.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_render_definitions.hpp"

namespace aetherion {
    std::vector<std::unique_ptr<ICommandBuffer>> VulkanCommandBuffer::allocateCommandBuffers(
        IGPUDevice& device, ICommandPool& commandPool, uint32_t count,
        const CommandGPUBufferDescription& description) {
        return VulkanCommandBuffer::allocateCommandBuffers(
            dynamic_cast<VulkanDevice&>(device), dynamic_cast<VulkanCommandPool&>(commandPool),
            count, description);
    }

    std::vector<std::unique_ptr<ICommandBuffer>> VulkanCommandBuffer::allocateCommandBuffers(
        VulkanDevice& device, VulkanCommandPool& commandPool, uint32_t count,
        const CommandGPUBufferDescription& description) {
        return VulkanCommandBuffer::allocateCommandBuffers(
            device.getVkDevice(), commandPool.getVkCommandPool(), count, description);
    }

    std::vector<std::unique_ptr<ICommandBuffer>> VulkanCommandBuffer::allocateCommandBuffers(
        vk::Device device, vk::CommandPool commandPool, uint32_t count,
        const CommandGPUBufferDescription& description) {
        std::vector<std::unique_ptr<ICommandBuffer>> commandBuffers;
        commandBuffers.reserve(count);

        vk::CommandBufferAllocateInfo allocateInfo
            = vk::CommandBufferAllocateInfo()
                  .setCommandPool(commandPool)
                  .setLevel(toVkCommandBufferLevel(description.level))
                  .setCommandBufferCount(count);

        auto result = device.allocateCommandBuffers(allocateInfo);

        for (const auto& commandBuffer : result) {
            commandBuffers.push_back(
                std::make_unique<VulkanCommandBuffer>(device, commandPool, commandBuffer));
        }

        return commandBuffers;
    }

    VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice& device, VulkanCommandPool& commandPool,
                                             const CommandGPUBufferDescription& description)
        : device_(device.getVkDevice()),
          commandPool_(commandPool.getVkCommandPool()),
          shouldFreeCommandBuffer_(commandPool.supportsFreeCommandBuffer()) {
        vk::CommandBufferAllocateInfo allocateInfo
            = vk::CommandBufferAllocateInfo()
                  .setCommandPool(commandPool_)
                  .setLevel(toVkCommandBufferLevel(description.level))
                  .setCommandBufferCount(1);

        auto result = device_.allocateCommandBuffers(allocateInfo);

        commandBuffer_ = result.front();
    }

    VulkanCommandBuffer::VulkanCommandBuffer(vk::Device device, vk::CommandPool commandPool,
                                             vk::CommandBuffer commandBuffer, bool shouldFree)
        : device_(device),
          commandPool_(commandPool),
          commandBuffer_(commandBuffer),
          shouldFreeCommandBuffer_(shouldFree) {}

    VulkanCommandBuffer::~VulkanCommandBuffer() noexcept { clear(); }

    VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept
        : ICommandBuffer(std::move(other)),
          device_(other.device_),
          commandPool_(other.commandPool_),
          commandBuffer_(other.commandBuffer_),
          shouldFreeCommandBuffer_(other.shouldFreeCommandBuffer_) {
        other.device_ = nullptr;
        other.commandPool_ = nullptr;
        other.commandBuffer_ = nullptr;
        other.shouldFreeCommandBuffer_ = false;
    }

    VulkanCommandBuffer& VulkanCommandBuffer::operator=(VulkanCommandBuffer&& other) noexcept {
        if (this != &other) {
            clear();

            ICommandBuffer::operator=(std::move(other));
            device_ = other.device_;
            commandPool_ = other.commandPool_;
            commandBuffer_ = other.commandBuffer_;
            shouldFreeCommandBuffer_ = other.shouldFreeCommandBuffer_;

            other.release();
        }
        return *this;
    }

    void VulkanCommandBuffer::clear() noexcept {
        if (commandBuffer_ && device_ && commandPool_ && shouldFreeCommandBuffer_) {
            device_.freeCommandBuffers(commandPool_, 1, &commandBuffer_);
            commandBuffer_ = nullptr;
        }
        device_ = nullptr;
        commandPool_ = nullptr;
        shouldFreeCommandBuffer_ = false;
    }

    void VulkanCommandBuffer::release() noexcept {
        commandBuffer_ = nullptr;
        device_ = nullptr;
        commandPool_ = nullptr;
        shouldFreeCommandBuffer_ = false;
    }

    void VulkanCommandBuffer::freeCommandBuffers(
        IGPUDevice& device, ICommandPool& commandPool,
        std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers) {
        std::vector<vk::CommandBuffer> vkCommandBuffers;
        vkCommandBuffers.reserve(commandBuffers.size());

        for (const auto& commandBuffer : commandBuffers) {
            auto& vkCommandBuffer = dynamic_cast<VulkanCommandBuffer&>(commandBuffer.get());
            vkCommandBuffers.push_back(vkCommandBuffer.getVkCommandBuffer());
            vkCommandBuffer.release();
        }

        VulkanCommandBuffer::freeCommandBuffers(
            dynamic_cast<VulkanDevice&>(device).getVkDevice(),
            dynamic_cast<VulkanCommandPool&>(commandPool).getVkCommandPool(), vkCommandBuffers);
    }

    void VulkanCommandBuffer::freeCommandBuffers(
        VulkanDevice& device, VulkanCommandPool& commandPool,
        std::span<std::reference_wrapper<VulkanCommandBuffer>> commandBuffers) {
        std::vector<vk::CommandBuffer> vkCommandBuffers;
        vkCommandBuffers.reserve(commandBuffers.size());

        for (auto& commandBuffer : commandBuffers) {
            vkCommandBuffers.push_back(commandBuffer.get().getVkCommandBuffer());
            commandBuffer.get().release();
        }

        VulkanCommandBuffer::freeCommandBuffers(device.getVkDevice(),
                                                commandPool.getVkCommandPool(), vkCommandBuffers);
    }

    void VulkanCommandBuffer::freeCommandBuffers(vk::Device device, vk::CommandPool commandPool,
                                                 std::span<vk::CommandBuffer> commandBuffers) {
        device.freeCommandBuffers(commandPool, commandBuffers);
    }

    constexpr vk::BufferCopy toVkBufferCopy(const BufferCopyRegion& region) {
        return vk::BufferCopy()
            .setSrcOffset(region.srcOffset)
            .setDstOffset(region.dstOffset)
            .setSize(region.size);
    }

    vk::RenderingAttachmentInfo toVkRenderingAttachmentInfo(
        const AttachmentDescription& attachment) {
        if (!attachment.image) {
            throw std::invalid_argument("Attachment image is null");
        }
        const auto* vkImage = dynamic_cast<VulkanImage*>(attachment.image);
        if (!attachment.imageView) {
            throw std::invalid_argument("Attachment image view is null");
        }
        const auto* vkImageView = dynamic_cast<VulkanImageView*>(attachment.imageView);
        // NOTE: Resolve image view is optional (depends on the attachment's resolve mode).
        const auto* vkResolveImageView
            = dynamic_cast<VulkanImageView*>(attachment.resolveImageView);

        auto vkAttachment
            = vk::RenderingAttachmentInfo()
                  .setImageView(vkImageView->getVkImageView())
                  .setImageLayout(toVkImageLayout(attachment.imageLayout))
                  .setLoadOp(toVkAttachmentLoadOp(attachment.loadOp))
                  .setStoreOp(toVkAttachmentStoreOp(attachment.storeOp))
                  .setClearValue(toVkClearValue(attachment.clearValue))
                  .setResolveMode(toVkResolveMode(attachment.resolveMode))
                  .setResolveImageLayout(toVkImageLayout(attachment.resolveImageLayout));

        if (attachment.resolveImageView) {
            if (!vkResolveImageView) {
                throw std::invalid_argument("Attachment resolve image view is null");
            }
            vkAttachment.setResolveImageView(vkResolveImageView->getVkImageView());
        }

        return vkAttachment;
    }

    vk::MemoryBarrier2 toVkMemoryBarrier2(const GeneralMemoryBarrierDescription& barrier) {
        return vk::MemoryBarrier2()
            .setSrcStageMask(toVkPipelineStageFlags(barrier.srcStageFlags))
            .setSrcAccessMask(toVkAccessFlags(barrier.srcAccessFlags))
            .setDstStageMask(toVkPipelineStageFlags(barrier.dstStageFlags))
            .setDstAccessMask(toVkAccessFlags(barrier.dstAccessFlags));
    }

    vk::BufferMemoryBarrier2 toVkBufferMemoryBarrier2(const BufferBarrierDescription& barrier,
                                                      vk::Buffer buffer) {
        return vk::BufferMemoryBarrier2()
            .setSrcStageMask(toVkPipelineStageFlags(barrier.srcStageFlags))
            .setSrcAccessMask(toVkAccessFlags(barrier.srcAccessFlags))
            .setDstStageMask(toVkPipelineStageFlags(barrier.dstStageFlags))
            .setDstAccessMask(toVkAccessFlags(barrier.dstAccessFlags))
            .setSrcQueueFamilyIndex(barrier.srcQueueFamilyIndex)
            .setDstQueueFamilyIndex(barrier.dstQueueFamilyIndex)
            .setBuffer(buffer)
            .setOffset(barrier.offset)
            .setSize(barrier.size);
    }

    vk::ImageMemoryBarrier2 toVkImageMemoryBarrier2(const ImageBarrierDescription& barrier,
                                                    vk::Image image) {
        return vk::ImageMemoryBarrier2()
            .setSrcStageMask(toVkPipelineStageFlags(barrier.srcStageFlags))
            .setSrcAccessMask(toVkAccessFlags(barrier.srcAccessFlags))
            .setDstStageMask(toVkPipelineStageFlags(barrier.dstStageFlags))
            .setDstAccessMask(toVkAccessFlags(barrier.dstAccessFlags))
            .setOldLayout(toVkImageLayout(barrier.oldLayout))
            .setNewLayout(toVkImageLayout(barrier.newLayout))
            .setSrcQueueFamilyIndex(barrier.srcQueueFamilyIndex)
            .setDstQueueFamilyIndex(barrier.dstQueueFamilyIndex)
            .setImage(image)
            .setSubresourceRange(toVkImageSubresourceRange(barrier.subresource));
    }

    void VulkanCommandBuffer::begin(CommandBufferUsageFlags flags) {
        commandBuffer_.begin(
            vk::CommandBufferBeginInfo().setFlags(toVkCommandBufferUsageFlags(flags)));
    }

    void VulkanCommandBuffer::reset(bool releaseResources) {
        commandBuffer_.reset(releaseResources ? vk::CommandBufferResetFlagBits::eReleaseResources
                                              : vk::CommandBufferResetFlags());
    }

    void VulkanCommandBuffer::end() { commandBuffer_.end(); }

    void VulkanCommandBuffer::beginRendering(const RenderDescription& renderDescription) {
        auto vkColorAttachments = std::vector<vk::RenderingAttachmentInfo>();
        vkColorAttachments.reserve(renderDescription.colorAttachments.size());
        for (const auto& attachment : renderDescription.colorAttachments) {
            vkColorAttachments.push_back(toVkRenderingAttachmentInfo(attachment));
        }

        auto vkRenderingInfo = vk::RenderingInfo()
                                   .setRenderArea(toVkRect2D(renderDescription.renderArea))
                                   .setLayerCount(renderDescription.layerCount)
                                   .setViewMask(renderDescription.viewMask)
                                   .setColorAttachments(vkColorAttachments);

        if (renderDescription.depthAttachment.has_value()) {
            auto vkDepthAttachment
                = toVkRenderingAttachmentInfo(*renderDescription.depthAttachment);
            vkRenderingInfo.setPDepthAttachment(&vkDepthAttachment);
        }
        if (renderDescription.stencilAttachment.has_value()) {
            auto vkStencilAttachment
                = toVkRenderingAttachmentInfo(*renderDescription.stencilAttachment);
            vkRenderingInfo.setPStencilAttachment(&vkStencilAttachment);
        }

        commandBuffer_.beginRendering(vkRenderingInfo);
    }

    void VulkanCommandBuffer::endRendering() { commandBuffer_.endRendering(); }

    void VulkanCommandBuffer::executeCommands(
        std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers) {
        std::vector<vk::CommandBuffer> vkCommandBuffers;
        vkCommandBuffers.reserve(commandBuffers.size());
        for (auto commandBuffer : commandBuffers) {
            const auto& vkCommandBuffer
                = dynamic_cast<const VulkanCommandBuffer&>(commandBuffer.get());
            vkCommandBuffers.push_back(vkCommandBuffer.getVkCommandBuffer());
        }

        commandBuffer_.executeCommands(vkCommandBuffers);
    }

    void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount,
                                   uint32_t firstVertex, uint32_t firstInstance) {
        commandBuffer_.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VulkanCommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                                          uint32_t firstIndex, uint32_t baseVertex,
                                          uint32_t firstInstance) {
        commandBuffer_.drawIndexed(indexCount, instanceCount, firstIndex,
                                   static_cast<int32_t>(baseVertex), firstInstance);
    }

    void VulkanCommandBuffer::dispatchCompute(uint32_t x, uint32_t y, uint32_t z) {
        commandBuffer_.dispatch(x, y, z);
    }

    void VulkanCommandBuffer::setViewport(Rect2Df viewport, float minDepth, float maxDepth) {
        commandBuffer_.setViewport(0, vk::Viewport()
                                          .setX(viewport.offset.x)
                                          .setY(viewport.offset.y)
                                          .setWidth(viewport.extent.width)
                                          .setHeight(viewport.extent.height)
                                          .setMinDepth(minDepth)
                                          .setMaxDepth(maxDepth));
    }

    void VulkanCommandBuffer::setScissor(Rect2Di scissor) {
        commandBuffer_.setScissor(0, toVkRect2D(scissor));
    }

    void VulkanCommandBuffer::clear(IGPUImage& image, GPUImageLayout layout,
                                    const std::vector<GPUImageRangeDescription>& ranges,
                                    const ClearValue& clearValue) {
        const auto& vkImage = dynamic_cast<VulkanImage&>(image);

        vk::ClearValue vkClearValue = toVkClearValue(clearValue);

        std::vector<vk::ImageSubresourceRange> vkRanges;
        vkRanges.reserve(ranges.size());

        // NOTE: vk::ClearValue is a naked union, so the warning about accessing inactive union
        // members is suppressed.
        if (clearValue.index() == 0) {
            // Color clear
            for (const auto& range : ranges)
                vkRanges.push_back(toVkImageSubresourceRange(
                    {.aspectMask = GPUImageAspect::Color, .range = range}));
            commandBuffer_.clearColorImage(
                vkImage.getVkImage(), toVkImageLayout(layout),
                vkClearValue.color,  // NOLINT(cppcoreguidelines-pro-type-union-access)
                vkRanges);
        } else if (clearValue.index() == 1) {
            // Depth-stencil clear
            for (const auto& range : ranges)
                vkRanges.push_back(toVkImageSubresourceRange(
                    {.aspectMask = GPUImageAspect::Depth, .range = range}));
            commandBuffer_.clearDepthStencilImage(
                vkImage.getVkImage(), toVkImageLayout(layout),
                vkClearValue.depthStencil,  // NOLINT(cppcoreguidelines-pro-type-union-access)
                vkRanges);
        } else {
            throw std::invalid_argument("Invalid ClearValue variant");
        }
    }

    void VulkanCommandBuffer::bindPipeline(IPipeline& pipeline) {
        const auto& vkPipeline = dynamic_cast<const VulkanPipeline&>(pipeline);

        vk::PipelineBindPoint bindpoint = toVkPipelineBindPoint(vkPipeline.getPipelineType());

        commandBuffer_.bindPipeline(bindpoint, vkPipeline.getVkPipeline());
    }

    void VulkanCommandBuffer::bindDescriptorSets(
        IPipelineLayout& pipelineLayout, PipelineBindPoint bindPoint, uint32_t firstSet,
        std::span<std::reference_wrapper<IDescriptorSet>> descriptorSets) {
        const auto& vkPipelineLayout = dynamic_cast<const VulkanPipelineLayout&>(pipelineLayout);

        std::vector<vk::DescriptorSet> vkDescriptorSets;
        vkDescriptorSets.reserve(descriptorSets.size());
        for (auto set : descriptorSets) {
            const auto& vkSet = dynamic_cast<const VulkanDescriptorSet&>(set.get());

            vkDescriptorSets.push_back(vkSet.getVkDescriptorSet());
        }

        vk::PipelineBindPoint vkBindPoint = toVkPipelineBindPoint(bindPoint);

        commandBuffer_.bindDescriptorSets(vkBindPoint, vkPipelineLayout.getVkPipelineLayout(),
                                          firstSet, vkDescriptorSets, {});
    }

    void VulkanCommandBuffer::pushConstantRange(IPipelineLayout& pipelineLayout,
                                                IPushConstantRange& pushConstantRange,
                                                std::span<const std::byte> data) {
        const auto& vkPipelineLayout = dynamic_cast<const VulkanPipelineLayout&>(pipelineLayout);
        const auto& vkPushConstantRange = dynamic_cast<VulkanPushConstantRange&>(pushConstantRange);

        commandBuffer_.pushConstants(vkPipelineLayout.getVkPipelineLayout(),
                                     vkPushConstantRange.getVkPushConstantRange().stageFlags,
                                     vkPushConstantRange.getVkPushConstantRange().offset,
                                     vkPushConstantRange.getVkPushConstantRange().size,
                                     data.data());
    }

    void VulkanCommandBuffer::bindVertexBuffers(
        uint32_t firstBinding, uint32_t bindingCount,
        std::span<const VertexBufferBindingDescription> bindings) {
        std::vector<vk::Buffer> vkBuffers;
        std::vector<vk::DeviceSize> vkOffsets;
        std::vector<vk::DeviceSize> vkSizes;
        std::vector<vk::DeviceSize> vkStrides;
        vkBuffers.reserve(bindingCount);
        vkOffsets.reserve(bindingCount);
        vkSizes.reserve(bindingCount);
        vkStrides.reserve(bindingCount);

        for (const auto& binding : bindings) {
            if (!binding.buffer) {
                throw std::invalid_argument("Vertex buffer is null.");
            }
            auto* vkBuffer = dynamic_cast<VulkanBuffer*>(binding.buffer);

            vkBuffers.push_back(vkBuffer->getVkBuffer());
            vkOffsets.push_back(binding.offset);
            vkSizes.push_back(binding.size);
            vkStrides.push_back(binding.stride);
        }

        commandBuffer_.bindVertexBuffers2(firstBinding, vkBuffers, vkOffsets, vkSizes, vkStrides);
    }

    void VulkanCommandBuffer::bindIndexBuffer(IGPUBuffer& buffer, size_t offset,
                                              IndexType indexType) {
        const auto& vkBuffer = dynamic_cast<const VulkanBuffer*>(&buffer);

        commandBuffer_.bindIndexBuffer(vkBuffer->getVkBuffer(), offset, toVkIndexType(indexType));
    }

    void VulkanCommandBuffer::copyBuffer(IGPUBuffer& src, IGPUBuffer& dst,
                                         const std::vector<BufferCopyRegion>& regions) {
        const auto& vkSrcBuffer = dynamic_cast<const VulkanBuffer&>(src);
        const auto& vkDstBuffer = dynamic_cast<const VulkanBuffer&>(dst);

        std::vector<vk::BufferCopy> vkRegions;
        vkRegions.reserve(regions.size());
        for (const auto& region : regions) {
            vkRegions.push_back(toVkBufferCopy(region));
        }

        commandBuffer_.copyBuffer(vkSrcBuffer.getVkBuffer(), vkDstBuffer.getVkBuffer(), vkRegions);
    }

    void VulkanCommandBuffer::barrier(
        std::span<const GeneralMemoryBarrierDescription> generalBarriers,
        std::span<const BufferBarrierDescription> bufferBarriers,
        std::span<const ImageBarrierDescription> imageBarriers) {
        auto vkGeneralBarriers = std::vector<vk::MemoryBarrier2>();
        vkGeneralBarriers.reserve(generalBarriers.size());
        for (const auto& barrier : generalBarriers) {
            vkGeneralBarriers.push_back(toVkMemoryBarrier2(barrier));
        }
        auto vkBufferBarriers = std::vector<vk::BufferMemoryBarrier2>();
        vkBufferBarriers.reserve(bufferBarriers.size());
        for (const auto& barrier : bufferBarriers) {
            if (!barrier.buffer) {
                throw std::invalid_argument("Buffer in BufferBarrierDescription is null.");
            }
            auto* vkBuffer = dynamic_cast<VulkanBuffer*>(barrier.buffer);

            vkBufferBarriers.push_back(toVkBufferMemoryBarrier2(barrier, vkBuffer->getVkBuffer()));
        }
        auto vkImageBarriers = std::vector<vk::ImageMemoryBarrier2>();
        vkImageBarriers.reserve(imageBarriers.size());
        for (const auto& barrier : imageBarriers) {
            if (!barrier.image) {
                throw std::invalid_argument("Image in ImageBarrierDescription is null.");
            }
            auto* vkImage = dynamic_cast<VulkanImage*>(barrier.image);

            vkImageBarriers.push_back(toVkImageMemoryBarrier2(barrier, vkImage->getVkImage()));
        }

        commandBuffer_.pipelineBarrier2(vk::DependencyInfo()
                                            .setMemoryBarriers(vkGeneralBarriers)
                                            .setBufferMemoryBarriers(vkBufferBarriers)
                                            .setImageMemoryBarriers(vkImageBarriers));
    }

    VulkanCommandPool::VulkanCommandPool(VulkanDevice& device,
                                         const CommandPoolDescription& description)
        : device_(device.getVkDevice()),
          freeCommandBufferSupport_(
              description.flags.contains(CommandPoolBehavior::ResetCommandBuffer)) {
        commandPool_
            = device_.createCommandPool(vk::CommandPoolCreateInfo()
                                            .setFlags(toVkCommandPoolCreateFlags(description.flags))
                                            .setQueueFamilyIndex(description.queueFamilyIndex));
    }

    VulkanCommandPool::VulkanCommandPool(vk::Device device, vk::CommandPool commandPool,
                                         bool freeCommandBufferSupport)
        : device_(device),
          commandPool_(commandPool),
          freeCommandBufferSupport_(freeCommandBufferSupport) {}

    VulkanCommandPool::~VulkanCommandPool() noexcept { clear(); }

    VulkanCommandPool::VulkanCommandPool(VulkanCommandPool&& other) noexcept
        : ICommandPool(std::move(other)),
          device_(other.device_),
          commandPool_(other.commandPool_),
          freeCommandBufferSupport_(other.freeCommandBufferSupport_) {
        other.device_ = nullptr;
        other.commandPool_ = nullptr;
        other.freeCommandBufferSupport_ = false;
    }

    VulkanCommandPool& VulkanCommandPool::operator=(VulkanCommandPool&& other) noexcept {
        if (this != &other) {
            clear();

            ICommandPool::operator=(std::move(other));
            device_ = other.device_;
            commandPool_ = other.commandPool_;
            freeCommandBufferSupport_ = other.freeCommandBufferSupport_;

            other.release();
        }
        return *this;
    }

    void VulkanCommandPool::clear() noexcept {
        if (commandPool_ && device_) {
            device_.destroyCommandPool(commandPool_);
            commandPool_ = nullptr;
        }
        device_ = nullptr;
        freeCommandBufferSupport_ = false;
    }

    void VulkanCommandPool::release() noexcept {
        commandPool_ = nullptr;
        device_ = nullptr;
        freeCommandBufferSupport_ = false;
    }

    void VulkanCommandPool::reset(bool releaseResources) {
        device_.resetCommandPool(commandPool_, releaseResources
                                                   ? vk::CommandPoolResetFlagBits::eReleaseResources
                                                   : vk::CommandPoolResetFlags());
    }
}  // namespace aetherion