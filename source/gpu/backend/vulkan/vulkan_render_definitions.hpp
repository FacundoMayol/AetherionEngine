#pragma once

#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "aetherion/gpu/backend/render_definitions.hpp"
#include "aetherion/util/common_definitions.hpp"

namespace aetherion {
    // --- Structs ---

    constexpr vk::Offset2D toVkOffset2D(const Offset2Di& offset) {
        return vk::Offset2D{offset.x, offset.y};
    }

    constexpr vk::Extent2D toVkExtent2D(const Extent2Du& extent) {
        return vk::Extent2D{extent.width, extent.height};
    }

    constexpr vk::Extent3D toVkExtent3D(const Extent3Du& extent) {
        return vk::Extent3D{extent.width, extent.height, extent.depth};
    }

    constexpr vk::Rect2D toVkRect2D(const Rect2Diu& rect) {
        const vk::Offset2D offset = toVkOffset2D(rect.offset);
        const vk::Extent2D extent = toVkExtent2D(rect.extent);
        return vk::Rect2D{offset, extent};
    }

    constexpr vk::ClearColorValue toVkClearColorValue(const ColorClearValue& color) {
        return std::visit([](const auto& arr) { return vk::ClearColorValue(arr); }, color);
    }

    constexpr vk::ClearDepthStencilValue toVkClearDepthStencilValue(float depth, uint32_t stencil) {
        return vk::ClearDepthStencilValue(depth, stencil);
    }

    constexpr vk::ClearValue toVkClearValue(const ClearValue& clearValue) {
        if (std::holds_alternative<ColorClearValue>(clearValue)) {
            return toVkClearColorValue(std::get<ColorClearValue>(clearValue));
        } else if (std::holds_alternative<DepthStencilValue>(clearValue)) {
            const auto& depthStencil = std::get<DepthStencilValue>(clearValue);
            return toVkClearDepthStencilValue(depthStencil.depth, depthStencil.stencil);
        } else {
            throw std::invalid_argument("Invalid ClearValue variant");
        }
    }

    // --- Queues ---

    constexpr vk::QueueFlagBits toVkQueueFlag(const GPUQueueType type) {
        switch (type) {
            case GPUQueueType::Graphics:
                return vk::QueueFlagBits::eGraphics;
            case GPUQueueType::Compute:
                return vk::QueueFlagBits::eCompute;
            case GPUQueueType::Transfer:
                return vk::QueueFlagBits::eTransfer;
            case GPUQueueType::Present:
                // NOTE: Vulkan does not have a specific Present flag; presentation support is
                // queried separately.
                return {};
            default:
                throw std::invalid_argument("Invalid GPUQueueType");
        }
    }

    constexpr GPUQueueType toQueueType(const vk::QueueFlagBits type) {
        switch (type) {
            case vk::QueueFlagBits::eGraphics:
                return GPUQueueType::Graphics;
            case vk::QueueFlagBits::eCompute:
                return GPUQueueType::Compute;
            case vk::QueueFlagBits::eTransfer:
                return GPUQueueType::Transfer;
            default:
                throw std::invalid_argument("Invalid vk::QueueFlagBits");
        }
    }

    constexpr vk::QueueFlags toVkQueueFlags(const GPUQueueTypeFlags flags) {
        vk::QueueFlags vkFlags = {};

        if (flags.contains(GPUQueueType::Graphics)) {
            vkFlags |= vk::QueueFlagBits::eGraphics;
        }
        if (flags.contains(GPUQueueType::Compute)) {
            vkFlags |= vk::QueueFlagBits::eCompute;
        }
        if (flags.contains(GPUQueueType::Transfer)) {
            vkFlags |= vk::QueueFlagBits::eTransfer;
        }
        if (flags.contains(GPUQueueType::Present)) {
            // NOTE: Vulkan does not have a specific Present flag; presentation support is queried
            // separately.
        }

        return vkFlags;
    }

    constexpr GPUQueueTypeFlags toQueueTypeFlags(const vk::QueueFlags flags) {
        GPUQueueTypeFlags queueFlags = {};

        if (flags & vk::QueueFlagBits::eGraphics) {
            queueFlags |= GPUQueueType::Graphics;
        }
        if (flags & vk::QueueFlagBits::eCompute) {
            queueFlags |= GPUQueueType::Compute;
        }
        if (flags & vk::QueueFlagBits::eTransfer) {
            queueFlags |= GPUQueueType::Transfer;
        }

        return queueFlags;
    }

    constexpr QueuePresentResultCode toQueuePresentResultCode(vk::Result result) {
        switch (result) {
            case vk::Result::eSuccess:
                return QueuePresentResultCode::Success;
            case vk::Result::eSuboptimalKHR:
                return QueuePresentResultCode::Suboptimal;
            case vk::Result::eErrorOutOfDateKHR:
                return QueuePresentResultCode::OutOfDate;
            case vk::Result::eErrorSurfaceLostKHR:
                return QueuePresentResultCode::SurfaceLost;
            case vk::Result::eErrorDeviceLost:
                return QueuePresentResultCode::DeviceLost;
            case vk::Result::eErrorUnknown:
                return QueuePresentResultCode::Error;
            default:
                return QueuePresentResultCode::Error;
        }
    }

    // --- Present ---

    constexpr vk::PresentModeKHR toVkPresentMode(const PresentMode mode) {
        switch (mode) {
            case PresentMode::Immediate:
                return vk::PresentModeKHR::eImmediate;
            case PresentMode::Mailbox:
                return vk::PresentModeKHR::eMailbox;
            case PresentMode::Fifo:
                return vk::PresentModeKHR::eFifo;
            case PresentMode::FifoRelaxed:
                return vk::PresentModeKHR::eFifoRelaxed;
            case PresentMode::SharedDemandRefresh:
                return vk::PresentModeKHR::eSharedDemandRefresh;
            case PresentMode::SharedContinuousRefresh:
                return vk::PresentModeKHR::eSharedContinuousRefresh;
            default:
                throw std::invalid_argument("Invalid PresentMode");
        }
    }

    // --- Swapchain ---

    constexpr vk::Result toVkSwapchainAcquireResult(const SwapchainAcquireResultCode code) {
        switch (code) {
            case SwapchainAcquireResultCode::Success:
                return vk::Result::eSuccess;
            case SwapchainAcquireResultCode::Suboptimal:
                return vk::Result::eSuboptimalKHR;
            case SwapchainAcquireResultCode::OutOfDate:
                return vk::Result::eErrorOutOfDateKHR;
            case SwapchainAcquireResultCode::SurfaceLost:
                return vk::Result::eErrorSurfaceLostKHR;
            case SwapchainAcquireResultCode::DeviceLost:
                return vk::Result::eErrorDeviceLost;
            case SwapchainAcquireResultCode::Error:
                return vk::Result::eErrorUnknown;
            default:
                throw std::invalid_argument("Invalid SwapchainAcquireResultCode");
        }
    }

    constexpr SwapchainAcquireResultCode toSwapchainAcquireResultCode(const vk::Result result) {
        switch (result) {
            case vk::Result::eSuccess:
                return SwapchainAcquireResultCode::Success;
            case vk::Result::eSuboptimalKHR:
                return SwapchainAcquireResultCode::Suboptimal;
            case vk::Result::eErrorOutOfDateKHR:
                return SwapchainAcquireResultCode::OutOfDate;
            case vk::Result::eErrorSurfaceLostKHR:
                return SwapchainAcquireResultCode::SurfaceLost;
            case vk::Result::eErrorDeviceLost:
                return SwapchainAcquireResultCode::DeviceLost;
            default:
                return SwapchainAcquireResultCode::Error;
        }
    }

    // --- Memory ---

    constexpr vma::AllocatorCreateFlagBits toVmaAllocatorFlag(const AllocatorProperty flag) {
        switch (flag) {
            case AllocatorProperty::None:
                return {};
            case AllocatorProperty::ExternallySynchronized:
                return vma::AllocatorCreateFlagBits::eExternallySynchronized;
            default:
                throw std::invalid_argument("Invalid AllocatorProperty");
        }
    }

    constexpr vma::AllocatorCreateFlags toVmaAllocatorFlags(const AllocatorPropertyFlags flags) {
        vma::AllocatorCreateFlags vmaFlags = {};

        if (flags.contains(AllocatorProperty::ExternallySynchronized)) {
            vmaFlags |= vma::AllocatorCreateFlagBits::eExternallySynchronized;
        }

        return vmaFlags;
    }

    constexpr vma::AllocationCreateFlagBits toVmaAllocationCreateFlag(
        const AllocationProperty flag) {
        switch (flag) {
            case AllocationProperty::None:
                return {};
            case AllocationProperty::DedicatedMemory:
                return vma::AllocationCreateFlagBits::eDedicatedMemory;
            case AllocationProperty::DoNotAllocate:
                return vma::AllocationCreateFlagBits::eNeverAllocate;
            case AllocationProperty::Mapped:
                return vma::AllocationCreateFlagBits::eMapped;
            case AllocationProperty::RandomAccess:
                return vma::AllocationCreateFlagBits::eHostAccessRandom;
            case AllocationProperty::SequentialAccess:
                return vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
            default:
                throw std::invalid_argument("Invalid AllocationProperty");
        }
    }

    constexpr vma::AllocationCreateFlags toVmaAllocationCreateFlags(
        const AllocationPropertyFlags flags) {
        vma::AllocationCreateFlags vmaFlags = {};

        if (flags.contains(AllocationProperty::DedicatedMemory)) {
            vmaFlags |= vma::AllocationCreateFlagBits::eDedicatedMemory;
        }
        if (flags.contains(AllocationProperty::DoNotAllocate)) {
            vmaFlags |= vma::AllocationCreateFlagBits::eNeverAllocate;
        }
        if (flags.contains(AllocationProperty::Mapped)) {
            vmaFlags |= vma::AllocationCreateFlagBits::eMapped;
        }
        if (flags.contains(AllocationProperty::RandomAccess)) {
            vmaFlags |= vma::AllocationCreateFlagBits::eHostAccessRandom;
        }
        if (flags.contains(AllocationProperty::SequentialAccess)) {
            vmaFlags |= vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
        }

        return vmaFlags;
    }

    constexpr vma::MemoryUsage toVmaMemoryUsage(const MemoryUsage usage) {
        switch (usage) {
            case MemoryUsage::PreferCpu:
                return vma::MemoryUsage::eCpuOnly;
            case MemoryUsage::PreferGpu:
                return vma::MemoryUsage::eGpuOnly;
            case MemoryUsage::Auto:
                return vma::MemoryUsage::eAuto;
            default:
                throw std::invalid_argument("Invalid MemoryUsage");
        }
    }

    constexpr vma::PoolCreateFlagBits toVmaAllocatorPoolFlag(const AllocatorPoolProperty flag) {
        switch (flag) {
            case AllocatorPoolProperty::None:
                return {};
            case AllocatorPoolProperty::IgnoreBufferImageGranularity:
                return vma::PoolCreateFlagBits::eIgnoreBufferImageGranularity;
            case AllocatorPoolProperty::Linear:
                return vma::PoolCreateFlagBits::eLinearAlgorithm;
            default:
                throw std::invalid_argument("Invalid AllocatorPoolProperty");
        }
    }

    constexpr vma::PoolCreateFlags toVmaAllocatorPoolFlags(const AllocatorPoolPropertyFlags flags) {
        vma::PoolCreateFlags vmaFlags = {};

        if (flags.contains(AllocatorPoolProperty::IgnoreBufferImageGranularity)) {
            vmaFlags |= vma::PoolCreateFlagBits::eIgnoreBufferImageGranularity;
        }
        if (flags.contains(AllocatorPoolProperty::Linear)) {
            vmaFlags |= vma::PoolCreateFlagBits::eLinearAlgorithm;
        }

        return vmaFlags;
    }

    // --- Command & Pipeline ---

    constexpr vk::CommandPoolCreateFlagBits toVkCommandPoolCreateFlag(
        const CommandPoolBehavior flag) {
        switch (flag) {
            case CommandPoolBehavior::None:
                return {};
            case CommandPoolBehavior::Transient:
                return vk::CommandPoolCreateFlagBits::eTransient;
            case CommandPoolBehavior::ResetCommandBuffer:
                return vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
            default:
                throw std::invalid_argument("Invalid CommandPoolBehavior");
        }
    }

    constexpr vk::CommandPoolCreateFlags toVkCommandPoolCreateFlags(
        const CommandPoolBehaviorFlags flags) {
        vk::CommandPoolCreateFlags vkFlags = {};

        if (flags.contains(CommandPoolBehavior::Transient)) {
            vkFlags |= vk::CommandPoolCreateFlagBits::eTransient;
        }
        if (flags.contains(CommandPoolBehavior::ResetCommandBuffer)) {
            vkFlags |= vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        }

        return vkFlags;
    }

    constexpr vk::CommandBufferUsageFlagBits toVkCommandBufferUsageFlag(
        const CommandBufferUsage usage) {
        switch (usage) {
            case CommandBufferUsage::OneTimeSubmit:
                return vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            case CommandBufferUsage::MultipleSubmit:
                return vk::CommandBufferUsageFlagBits::eSimultaneousUse;
            case CommandBufferUsage::Secondary:
                return vk::CommandBufferUsageFlagBits::eRenderPassContinue;
            default:
                throw std::invalid_argument("Invalid CommandBufferUsage");
        }
    }

    constexpr vk::CommandBufferUsageFlags toVkCommandBufferUsageFlags(
        const CommandBufferUsageFlags flags) {
        vk::CommandBufferUsageFlags vkFlags = {};

        if (flags.contains(CommandBufferUsage::OneTimeSubmit)) {
            vkFlags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        }
        if (flags.contains(CommandBufferUsage::MultipleSubmit)) {
            vkFlags |= vk::CommandBufferUsageFlagBits::eSimultaneousUse;
        }
        if (flags.contains(CommandBufferUsage::Secondary)) {
            vkFlags |= vk::CommandBufferUsageFlagBits::eRenderPassContinue;
        }

        return vkFlags;
    }

    constexpr vk::CommandBufferLevel toVkCommandBufferLevel(const CommandBufferLevel level) {
        switch (level) {
            case CommandBufferLevel::Primary:
                return vk::CommandBufferLevel::ePrimary;
            case CommandBufferLevel::Secondary:
                return vk::CommandBufferLevel::eSecondary;
            default:
                throw std::invalid_argument("Invalid CommandBufferLevel");
        }
    }

    constexpr vk::PipelineBindPoint toVkPipelineBindPoint(const PipelineBindPoint type) {
        switch (type) {
            case PipelineBindPoint::Graphics:
                return vk::PipelineBindPoint::eGraphics;
            case PipelineBindPoint::Compute:
                return vk::PipelineBindPoint::eCompute;
            default:
                throw std::invalid_argument("Invalid PipelineBindPoint");
        }
    }

    constexpr vk::PipelineStageFlagBits2 toVkPipelineStageFlag(const PipelineStage stage) {
        switch (stage) {
            case PipelineStage::None:
                return {};
            case PipelineStage::TopOfPipe:
                return vk::PipelineStageFlagBits2::eTopOfPipe;
            case PipelineStage::DrawIndirect:
                return vk::PipelineStageFlagBits2::eDrawIndirect;
            case PipelineStage::VertexInput:
                return vk::PipelineStageFlagBits2::eVertexInput;
            case PipelineStage::VertexShader:
                return vk::PipelineStageFlagBits2::eVertexShader;
            case PipelineStage::FragmentShader:
                return vk::PipelineStageFlagBits2::eFragmentShader;
            case PipelineStage::ComputeShader:
                return vk::PipelineStageFlagBits2::eComputeShader;
            case PipelineStage::GeometryShader:
                return vk::PipelineStageFlagBits2::eGeometryShader;
            case PipelineStage::TessellationControlShader:
                return vk::PipelineStageFlagBits2::eTessellationControlShader;
            case PipelineStage::TessellationEvaluationShader:
                return vk::PipelineStageFlagBits2::eTessellationEvaluationShader;
            case PipelineStage::EarlyFragmentTests:
                return vk::PipelineStageFlagBits2::eEarlyFragmentTests;
            case PipelineStage::LateFragmentTests:
                return vk::PipelineStageFlagBits2::eLateFragmentTests;
            case PipelineStage::ColorAttachmentOutput:
                return vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            case PipelineStage::Transfer:
                return vk::PipelineStageFlagBits2::eTransfer;
            case PipelineStage::BottomOfPipe:
                return vk::PipelineStageFlagBits2::eBottomOfPipe;
            case PipelineStage::Host:
                return vk::PipelineStageFlagBits2::eHost;
            case PipelineStage::AllGraphics:
                return vk::PipelineStageFlagBits2::eAllGraphics;
            case PipelineStage::AllCommands:
                return vk::PipelineStageFlagBits2::eAllCommands;
            default:
                throw std::invalid_argument("Invalid PipelineStage");
        }
    }

    constexpr vk::PipelineStageFlags2 toVkPipelineStageFlags(const PipelineStageFlags stages) {
        vk::PipelineStageFlags2 vkFlags = {};

        if (stages.contains(PipelineStage::None)) {
            vkFlags |= {};
        }
        if (stages.contains(PipelineStage::TopOfPipe)) {
            vkFlags |= vk::PipelineStageFlagBits2::eTopOfPipe;
        }
        if (stages.contains(PipelineStage::DrawIndirect)) {
            vkFlags |= vk::PipelineStageFlagBits2::eDrawIndirect;
        }
        if (stages.contains(PipelineStage::VertexInput)) {
            vkFlags |= vk::PipelineStageFlagBits2::eVertexInput;
        }
        if (stages.contains(PipelineStage::VertexShader)) {
            vkFlags |= vk::PipelineStageFlagBits2::eVertexShader;
        }
        if (stages.contains(PipelineStage::FragmentShader)) {
            vkFlags |= vk::PipelineStageFlagBits2::eFragmentShader;
        }
        if (stages.contains(PipelineStage::ComputeShader)) {
            vkFlags |= vk::PipelineStageFlagBits2::eComputeShader;
        }
        if (stages.contains(PipelineStage::GeometryShader)) {
            vkFlags |= vk::PipelineStageFlagBits2::eGeometryShader;
        }
        if (stages.contains(PipelineStage::TessellationControlShader)) {
            vkFlags |= vk::PipelineStageFlagBits2::eTessellationControlShader;
        }
        if (stages.contains(PipelineStage::TessellationEvaluationShader)) {
            vkFlags |= vk::PipelineStageFlagBits2::eTessellationEvaluationShader;
        }
        if (stages.contains(PipelineStage::EarlyFragmentTests)) {
            vkFlags |= vk::PipelineStageFlagBits2::eEarlyFragmentTests;
        }
        if (stages.contains(PipelineStage::LateFragmentTests)) {
            vkFlags |= vk::PipelineStageFlagBits2::eLateFragmentTests;
        }
        if (stages.contains(PipelineStage::ColorAttachmentOutput)) {
            vkFlags |= vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        }
        if (stages.contains(PipelineStage::Transfer)) {
            vkFlags |= vk::PipelineStageFlagBits2::eTransfer;
        }
        if (stages.contains(PipelineStage::BottomOfPipe)) {
            vkFlags |= vk::PipelineStageFlagBits2::eBottomOfPipe;
        }
        if (stages.contains(PipelineStage::Host)) {
            vkFlags |= vk::PipelineStageFlagBits2::eHost;
        }
        if (stages.contains(PipelineStage::AllGraphics)) {
            vkFlags |= vk::PipelineStageFlagBits2::eAllGraphics;
        }
        if (stages.contains(PipelineStage::AllCommands)) {
            vkFlags |= vk::PipelineStageFlagBits2::eAllCommands;
        }

        return vkFlags;
    }

    constexpr vk::AccessFlagBits2 toVkAccessFlag(const AccessType access) {
        switch (access) {
            case AccessType::None:
                return {};
            case AccessType::IndirectCommandRead:
                return vk::AccessFlagBits2::eIndirectCommandRead;
            case AccessType::IndexRead:
                return vk::AccessFlagBits2::eIndexRead;
            case AccessType::VertexAttributeRead:
                return vk::AccessFlagBits2::eVertexAttributeRead;
            case AccessType::UniformRead:
                return vk::AccessFlagBits2::eUniformRead;
            case AccessType::InputAttachmentRead:
                return vk::AccessFlagBits2::eInputAttachmentRead;
            case AccessType::ShaderRead:
                return vk::AccessFlagBits2::eShaderRead;
            case AccessType::ShaderWrite:
                return vk::AccessFlagBits2::eShaderWrite;
            case AccessType::ColorAttachmentRead:
                return vk::AccessFlagBits2::eColorAttachmentRead;
            case AccessType::ColorAttachmentWrite:
                return vk::AccessFlagBits2::eColorAttachmentWrite;
            case AccessType::DepthStencilAttachmentRead:
                return vk::AccessFlagBits2::eDepthStencilAttachmentRead;
            case AccessType::DepthStencilAttachmentWrite:
                return vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
            case AccessType::TransferRead:
                return vk::AccessFlagBits2::eTransferRead;
            case AccessType::TransferWrite:
                return vk::AccessFlagBits2::eTransferWrite;
            case AccessType::HostRead:
                return vk::AccessFlagBits2::eHostRead;
            case AccessType::HostWrite:
                return vk::AccessFlagBits2::eHostWrite;
            case AccessType::MemoryRead:
                return vk::AccessFlagBits2::eMemoryRead;
            case AccessType::MemoryWrite:
                return vk::AccessFlagBits2::eMemoryWrite;
            default:
                throw std::invalid_argument("Invalid AccessType");
        }
    }

    constexpr vk::AccessFlags2 toVkAccessFlags(const AccessTypeFlags access) {
        vk::AccessFlags2 flags = {};
        if (access.contains(AccessType::None)) {
            flags |= vk::AccessFlagBits2::eNone;
        }
        if (access.contains(AccessType::IndirectCommandRead)) {
            flags |= vk::AccessFlagBits2::eIndirectCommandRead;
        }
        if (access.contains(AccessType::IndexRead)) {
            flags |= vk::AccessFlagBits2::eIndexRead;
        }
        if (access.contains(AccessType::VertexAttributeRead)) {
            flags |= vk::AccessFlagBits2::eVertexAttributeRead;
        }
        if (access.contains(AccessType::UniformRead)) {
            flags |= vk::AccessFlagBits2::eUniformRead;
        }
        if (access.contains(AccessType::InputAttachmentRead)) {
            flags |= vk::AccessFlagBits2::eInputAttachmentRead;
        }
        if (access.contains(AccessType::ShaderRead)) {
            flags |= vk::AccessFlagBits2::eShaderRead;
        }
        if (access.contains(AccessType::ShaderWrite)) {
            flags |= vk::AccessFlagBits2::eShaderWrite;
        }
        if (access.contains(AccessType::ColorAttachmentRead)) {
            flags |= vk::AccessFlagBits2::eColorAttachmentRead;
        }
        if (access.contains(AccessType::ColorAttachmentWrite)) {
            flags |= vk::AccessFlagBits2::eColorAttachmentWrite;
        }
        if (access.contains(AccessType::DepthStencilAttachmentRead)) {
            flags |= vk::AccessFlagBits2::eDepthStencilAttachmentRead;
        }
        if (access.contains(AccessType::DepthStencilAttachmentWrite)) {
            flags |= vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
        }
        if (access.contains(AccessType::TransferRead)) {
            flags |= vk::AccessFlagBits2::eTransferRead;
        }
        if (access.contains(AccessType::TransferWrite)) {
            flags |= vk::AccessFlagBits2::eTransferWrite;
        }
        if (access.contains(AccessType::HostRead)) {
            flags |= vk::AccessFlagBits2::eHostRead;
        }
        if (access.contains(AccessType::HostWrite)) {
            flags |= vk::AccessFlagBits2::eHostWrite;
        }
        if (access.contains(AccessType::MemoryRead)) {
            flags |= vk::AccessFlagBits2::eMemoryRead;
        }
        if (access.contains(AccessType::MemoryWrite)) {
            flags |= vk::AccessFlagBits2::eMemoryWrite;
        }
        return flags;
    }

    // --- Shader ---

    constexpr vk::ShaderStageFlagBits toVkShaderStageFlag(const ShaderStage stage) {
        switch (stage) {
            case ShaderStage::None:
                return {};
            case ShaderStage::Vertex:
                return vk::ShaderStageFlagBits::eVertex;
            case ShaderStage::Fragment:
                return vk::ShaderStageFlagBits::eFragment;
            case ShaderStage::Compute:
                return vk::ShaderStageFlagBits::eCompute;
            case ShaderStage::Geometry:
                return vk::ShaderStageFlagBits::eGeometry;
            case ShaderStage::TessellationControl:
                return vk::ShaderStageFlagBits::eTessellationControl;
            case ShaderStage::TessellationEvaluation:
                return vk::ShaderStageFlagBits::eTessellationEvaluation;
            default:
                throw std::invalid_argument("Invalid ShaderStage");
        }
    }

    constexpr vk::ShaderStageFlags toVkShaderStageFlags(const ShaderStageFlags stages) {
        vk::ShaderStageFlags flags = {};
        if (stages.contains(ShaderStage::Vertex)) {
            flags |= vk::ShaderStageFlagBits::eVertex;
        }
        if (stages.contains(ShaderStage::Fragment)) {
            flags |= vk::ShaderStageFlagBits::eFragment;
        }
        if (stages.contains(ShaderStage::Compute)) {
            flags |= vk::ShaderStageFlagBits::eCompute;
        }
        if (stages.contains(ShaderStage::Geometry)) {
            flags |= vk::ShaderStageFlagBits::eGeometry;
        }
        if (stages.contains(ShaderStage::TessellationControl)) {
            flags |= vk::ShaderStageFlagBits::eTessellationControl;
        }
        if (stages.contains(ShaderStage::TessellationEvaluation)) {
            flags |= vk::ShaderStageFlagBits::eTessellationEvaluation;
        }
        return flags;
    }

    // --- Descriptor ---

    constexpr vk::DescriptorType toVkDescriptorType(const DescriptorType type) {
        switch (type) {
            case DescriptorType::Sampler:
                return vk::DescriptorType::eSampler;
            case DescriptorType::CombinedImageSampler:
                return vk::DescriptorType::eCombinedImageSampler;
            case DescriptorType::SampledImage:
                return vk::DescriptorType::eSampledImage;
            case DescriptorType::StorageImage:
                return vk::DescriptorType::eStorageImage;
            case DescriptorType::UniformTexelBuffer:
                return vk::DescriptorType::eUniformTexelBuffer;
            case DescriptorType::StorageTexelBuffer:
                return vk::DescriptorType::eStorageTexelBuffer;
            case DescriptorType::UniformBuffer:
                return vk::DescriptorType::eUniformBuffer;
            case DescriptorType::StorageBuffer:
                return vk::DescriptorType::eStorageBuffer;
            case DescriptorType::InputAttachment:
                return vk::DescriptorType::eInputAttachment;
            default:
                throw std::invalid_argument("Invalid DescriptorType");
        }
    }

    constexpr vk::DescriptorPoolCreateFlagBits toVkDescriptorPoolCreateFlag(
        const DescriptorPoolBehavior flag) {
        switch (flag) {
            case DescriptorPoolBehavior::None:
                return {};
            case DescriptorPoolBehavior::FreeIndividualSets:
                return vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
            default:
                throw std::invalid_argument("Invalid DescriptorPoolBehavior");
        }
    }

    constexpr vk::DescriptorPoolCreateFlags toVkDescriptorPoolCreateFlags(
        const DescriptorPoolBehaviorFlags flags) {
        vk::DescriptorPoolCreateFlags vkFlags = {};

        if (flags.contains(DescriptorPoolBehavior::FreeIndividualSets)) {
            vkFlags |= vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        }

        return vkFlags;
    }

    // --- Resources ---

    constexpr vk::SharingMode toVkSharingMode(const SharingMode mode) {
        switch (mode) {
            case SharingMode::Exclusive:
                return vk::SharingMode::eExclusive;
            case SharingMode::Concurrent:
                return vk::SharingMode::eConcurrent;
            default:
                throw std::invalid_argument("Invalid SharingMode");
        }
    }

    constexpr vk::Format toVkFormat(const Format format) {
        switch (format) {
            case Format::Undefined:
                return vk::Format::eUndefined;
            case Format::R8Unorm:
                return vk::Format::eR8Unorm;
            case Format::R8Uint:
                return vk::Format::eR8Uint;
            case Format::R8Srgb:
                return vk::Format::eR8Srgb;
            case Format::R16Unorm:
                return vk::Format::eR16Unorm;
            case Format::R16Uint:
                return vk::Format::eR16Uint;
            case Format::R16Sfloat:
                return vk::Format::eR16Sfloat;
            case Format::R32Uint:
                return vk::Format::eR32Uint;
            case Format::R32Sfloat:
                return vk::Format::eR32Sfloat;
            case Format::R8G8Unorm:
                return vk::Format::eR8G8Unorm;
            case Format::R8G8Uint:
                return vk::Format::eR8G8Uint;
            case Format::R8G8Srgb:
                return vk::Format::eR8G8Srgb;
            case Format::R16G16Unorm:
                return vk::Format::eR16G16Unorm;
            case Format::R16G16Uint:
                return vk::Format::eR16G16Uint;
            case Format::R16G16Sfloat:
                return vk::Format::eR16G16Sfloat;
            case Format::R32G32Uint:
                return vk::Format::eR32G32Uint;
            case Format::R32G32Sfloat:
                return vk::Format::eR32G32Sfloat;
            case Format::R8G8B8A8Unorm:
                return vk::Format::eR8G8B8A8Unorm;
            case Format::R8G8B8A8Uint:
                return vk::Format::eR8G8B8A8Uint;
            case Format::R8G8B8A8Srgb:
                return vk::Format::eR8G8B8A8Srgb;
            case Format::B8G8R8A8Unorm:
                return vk::Format::eB8G8R8A8Unorm;
            case Format::B8G8R8A8Srgb:
                return vk::Format::eB8G8R8A8Srgb;
            case Format::R16G16B16A16Sfloat:
                return vk::Format::eR16G16B16A16Sfloat;
            case Format::R32G32B32A32Sfloat:
                return vk::Format::eR32G32B32A32Sfloat;
            case Format::D16Unorm:
                return vk::Format::eD16Unorm;
            case Format::D32Sfloat:
                return vk::Format::eD32Sfloat;
            case Format::D24UnormS8Uint:
                return vk::Format::eD24UnormS8Uint;
            default:
                throw std::invalid_argument("Invalid Format");
        }
    }

    constexpr Format toFormat(const vk::Format format) {
        switch (format) {
            case vk::Format::eUndefined:
                return Format::Undefined;
            case vk::Format::eR8Unorm:
                return Format::R8Unorm;
            case vk::Format::eR8Uint:
                return Format::R8Uint;
            case vk::Format::eR8Srgb:
                return Format::R8Srgb;
            case vk::Format::eR16Unorm:
                return Format::R16Unorm;
            case vk::Format::eR16Uint:
                return Format::R16Uint;
            case vk::Format::eR16Sfloat:
                return Format::R16Sfloat;
            case vk::Format::eR32Uint:
                return Format::R32Uint;
            case vk::Format::eR32Sfloat:
                return Format::R32Sfloat;
            case vk::Format::eR8G8Unorm:
                return Format::R8G8Unorm;
            case vk::Format::eR8G8Uint:
                return Format::R8G8Uint;
            case vk::Format::eR8G8Srgb:
                return Format::R8G8Srgb;
            case vk::Format::eR16G16Unorm:
                return Format::R16G16Unorm;
            case vk::Format::eR16G16Uint:
                return Format::R16G16Uint;
            case vk::Format::eR16G16Sfloat:
                return Format::R16G16Sfloat;
            case vk::Format::eR32G32Uint:
                return Format::R32G32Uint;
            case vk::Format::eR32G32Sfloat:
                return Format::R32G32Sfloat;
            case vk::Format::eR8G8B8A8Unorm:
                return Format::R8G8B8A8Unorm;
            case vk::Format::eR8G8B8A8Uint:
                return Format::R8G8B8A8Uint;
            case vk::Format::eR8G8B8A8Srgb:
                return Format::R8G8B8A8Srgb;
            case vk::Format::eB8G8R8A8Unorm:
                return Format::B8G8R8A8Unorm;
            case vk::Format::eB8G8R8A8Srgb:
                return Format::B8G8R8A8Srgb;
            case vk::Format::eR16G16B16A16Sfloat:
                return Format::R16G16B16A16Sfloat;
            case vk::Format::eR32G32B32A32Sfloat:
                return Format::R32G32B32A32Sfloat;
            case vk::Format::eD16Unorm:
                return Format::D16Unorm;
            case vk::Format::eD32Sfloat:
                return Format::D32Sfloat;
            case vk::Format::eD24UnormS8Uint:
                return Format::D24UnormS8Uint;
            default:
                throw std::invalid_argument("Invalid VkFormat");
        }
    }

    // --- Buffer ---

    constexpr vk::BufferUsageFlags toVkBufferUsageFlags(const GPUBufferUsageFlags usage) {
        vk::BufferUsageFlags vkFlags = {};

        if (usage.contains(GPUBufferUsage::TransferSrc)) {
            vkFlags |= vk::BufferUsageFlagBits::eTransferSrc;
        }
        if (usage.contains(GPUBufferUsage::TransferDst)) {
            vkFlags |= vk::BufferUsageFlagBits::eTransferDst;
        }
        if (usage.contains(GPUBufferUsage::UniformTexel)) {
            vkFlags |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
        }
        if (usage.contains(GPUBufferUsage::StorageTexel)) {
            vkFlags |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
        }
        if (usage.contains(GPUBufferUsage::Uniform)) {
            vkFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
        }
        if (usage.contains(GPUBufferUsage::Storage)) {
            vkFlags |= vk::BufferUsageFlagBits::eStorageBuffer;
        }
        if (usage.contains(GPUBufferUsage::Index)) {
            vkFlags |= vk::BufferUsageFlagBits::eIndexBuffer;
        }
        if (usage.contains(GPUBufferUsage::Vertex)) {
            vkFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
        }
        if (usage.contains(GPUBufferUsage::Indirect)) {
            vkFlags |= vk::BufferUsageFlagBits::eIndirectBuffer;
        }

        return vkFlags;
    }

    constexpr vk::BufferUsageFlags toVkBufferUsageFlags(const GPUBufferType type) {
        switch (type) {
            case GPUBufferType::Vertex:
                return vk::BufferUsageFlagBits::eVertexBuffer;
            case GPUBufferType::Index:
                return vk::BufferUsageFlagBits::eIndexBuffer;
            case GPUBufferType::Uniform:
                return vk::BufferUsageFlagBits::eUniformBuffer;
            case GPUBufferType::Storage:
                return vk::BufferUsageFlagBits::eStorageBuffer;
            default:
                throw std::invalid_argument("Invalid GPUBufferType");
        }
    }

    // --- Image ---

    constexpr vk::ImageLayout toVkImageLayout(const GPUImageLayout layout) {
        switch (layout) {
            case GPUImageLayout::Undefined:
                return vk::ImageLayout::eUndefined;
            case GPUImageLayout::General:
                return vk::ImageLayout::eGeneral;
            case GPUImageLayout::ColorAttachmentOptimal:
                return vk::ImageLayout::eColorAttachmentOptimal;
            case GPUImageLayout::DepthStencilAttachmentOptimal:
                return vk::ImageLayout::eDepthStencilAttachmentOptimal;
            case GPUImageLayout::DepthStencilReadOnlyOptimal:
                return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
            case GPUImageLayout::ShaderReadOnlyOptimal:
                return vk::ImageLayout::eShaderReadOnlyOptimal;
            case GPUImageLayout::TransferSrcOptimal:
                return vk::ImageLayout::eTransferSrcOptimal;
            case GPUImageLayout::TransferDstOptimal:
                return vk::ImageLayout::eTransferDstOptimal;
            case GPUImageLayout::Preinitialized:
                return vk::ImageLayout::ePreinitialized;
            case GPUImageLayout::PresentSource:
                return vk::ImageLayout::ePresentSrcKHR;
            default:
                throw std::invalid_argument("Invalid GPUImageLayout");
        }
    }

    constexpr vk::ImageType toVkImageType(const GPUImageType type) {
        switch (type) {
            case GPUImageType::Tex1d:
                return vk::ImageType::e1D;
            case GPUImageType::Tex2d:
                return vk::ImageType::e2D;
            case GPUImageType::Tex3d:
                return vk::ImageType::e3D;
            default:
                throw std::invalid_argument("Invalid GPUImageType");
        }
    }

    constexpr vk::ColorSpaceKHR toVkColorSpace(const ColorSpace space) {
        switch (space) {
            case ColorSpace::SrgbNonlinear:
                return vk::ColorSpaceKHR::eSrgbNonlinear;
            case ColorSpace::DisplayP3Nonlinear:
                return vk::ColorSpaceKHR::eDisplayP3NonlinearEXT;
            case ColorSpace::ExtendedSrgbLinear:
                return vk::ColorSpaceKHR::eExtendedSrgbLinearEXT;
            case ColorSpace::ExtendedSrgbNonlinear:
                return vk::ColorSpaceKHR::eExtendedSrgbNonlinearEXT;
            case ColorSpace::Bt709Linear:
                return vk::ColorSpaceKHR::eBt709LinearEXT;
            case ColorSpace::Bt709Nonlinear:
                return vk::ColorSpaceKHR::eBt709NonlinearEXT;
            case ColorSpace::Bt2020Linear:
                return vk::ColorSpaceKHR::eBt2020LinearEXT;
            case ColorSpace::DciP3Nonlinear:
                return vk::ColorSpaceKHR::eDciP3NonlinearEXT;
            case ColorSpace::Hdr10St2084:
                return vk::ColorSpaceKHR::eHdr10St2084EXT;
            case ColorSpace::Unknown:
            default:
                throw std::invalid_argument("Invalid ColorSpace");
        }
    }

    constexpr ColorSpace toColorSpace(const vk::ColorSpaceKHR space) {
        switch (space) {
            case vk::ColorSpaceKHR::eSrgbNonlinear:
                return ColorSpace::SrgbNonlinear;
            case vk::ColorSpaceKHR::eDisplayP3NonlinearEXT:
                return ColorSpace::DisplayP3Nonlinear;
            case vk::ColorSpaceKHR::eExtendedSrgbLinearEXT:
                return ColorSpace::ExtendedSrgbLinear;
            case vk::ColorSpaceKHR::eExtendedSrgbNonlinearEXT:
                return ColorSpace::ExtendedSrgbNonlinear;
            case vk::ColorSpaceKHR::eBt709LinearEXT:
                return ColorSpace::Bt709Linear;
            case vk::ColorSpaceKHR::eBt709NonlinearEXT:
                return ColorSpace::Bt709Nonlinear;
            case vk::ColorSpaceKHR::eBt2020LinearEXT:
                return ColorSpace::Bt2020Linear;
            case vk::ColorSpaceKHR::eDciP3NonlinearEXT:
                return ColorSpace::DciP3Nonlinear;
            case vk::ColorSpaceKHR::eHdr10St2084EXT:
                return ColorSpace::Hdr10St2084;
            default:
                throw std::invalid_argument("Invalid VkColorSpaceKHR");
        }
    }

    constexpr vk::ImageViewType toVkImageViewType(const GPUImageViewType type) {
        switch (type) {
            case GPUImageViewType::Tex1d:
                return vk::ImageViewType::e1D;
            case GPUImageViewType::Tex1dArray:
                return vk::ImageViewType::e1DArray;
            case GPUImageViewType::Tex2d:
                return vk::ImageViewType::e2D;
            case GPUImageViewType::Tex2dArray:
                return vk::ImageViewType::e2DArray;
            case GPUImageViewType::Tex3d:
                return vk::ImageViewType::e3D;
            case GPUImageViewType::TexCube:
                return vk::ImageViewType::eCube;
            case GPUImageViewType::TexCubeArray:
                return vk::ImageViewType::eCubeArray;
            default:
                throw std::invalid_argument("Invalid GPUImageViewType");
        }
    }

    constexpr vk::ComponentSwizzle toVkComponentSwizzle(const ComponentSwizzle swizzle) {
        switch (swizzle) {
            case ComponentSwizzle::Identity:
                return vk::ComponentSwizzle::eIdentity;
            case ComponentSwizzle::Zero:
                return vk::ComponentSwizzle::eZero;
            case ComponentSwizzle::One:
                return vk::ComponentSwizzle::eOne;
            case ComponentSwizzle::R:
                return vk::ComponentSwizzle::eR;
            case ComponentSwizzle::G:
                return vk::ComponentSwizzle::eG;
            case ComponentSwizzle::B:
                return vk::ComponentSwizzle::eB;
            case ComponentSwizzle::A:
                return vk::ComponentSwizzle::eA;
            default:
                throw std::invalid_argument("Invalid ComponentSwizzle");
        }
    }

    constexpr vk::ImageUsageFlagBits toVkImageUsageFlags(const GPUImageUsage usage) {
        switch (usage) {
            case GPUImageUsage::None:
                return {};
            case GPUImageUsage::Sampled:
                return vk::ImageUsageFlagBits::eSampled;
            case GPUImageUsage::Storage:
                return vk::ImageUsageFlagBits::eStorage;
            case GPUImageUsage::TransferSrc:
                return vk::ImageUsageFlagBits::eTransferSrc;
            case GPUImageUsage::TransferDst:
                return vk::ImageUsageFlagBits::eTransferDst;
            case GPUImageUsage::ColorAttachment:
                return vk::ImageUsageFlagBits::eColorAttachment;
            case GPUImageUsage::DepthStencilAttachment:
                return vk::ImageUsageFlagBits::eDepthStencilAttachment;
            case GPUImageUsage::InputAttachment:
                return vk::ImageUsageFlagBits::eInputAttachment;
            case GPUImageUsage::TransientAttachment:
                return vk::ImageUsageFlagBits::eTransientAttachment;
            default:
                throw std::invalid_argument("Invalid GPUImageUsage");
        }
    }

    constexpr vk::ImageUsageFlags toVkImageUsageFlags(const GPUImageUsageFlags usages) {
        vk::ImageUsageFlags flags = {};
        if (usages.contains(GPUImageUsage::Sampled)) {
            flags |= vk::ImageUsageFlagBits::eSampled;
        }
        if (usages.contains(GPUImageUsage::Storage)) {
            flags |= vk::ImageUsageFlagBits::eStorage;
        }
        if (usages.contains(GPUImageUsage::TransferSrc)) {
            flags |= vk::ImageUsageFlagBits::eTransferSrc;
        }
        if (usages.contains(GPUImageUsage::TransferDst)) {
            flags |= vk::ImageUsageFlagBits::eTransferDst;
        }
        if (usages.contains(GPUImageUsage::ColorAttachment)) {
            flags |= vk::ImageUsageFlagBits::eColorAttachment;
        }
        if (usages.contains(GPUImageUsage::DepthStencilAttachment)) {
            flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
        }
        if (usages.contains(GPUImageUsage::InputAttachment)) {
            flags |= vk::ImageUsageFlagBits::eInputAttachment;
        }
        if (usages.contains(GPUImageUsage::TransientAttachment)) {
            flags |= vk::ImageUsageFlagBits::eTransientAttachment;
        }
        return flags;
    }

    constexpr vk::ImageTiling toVkImageTiling(const GPUImageTiling tiling) {
        switch (tiling) {
            case GPUImageTiling::Optimal:
                return vk::ImageTiling::eOptimal;
            case GPUImageTiling::Linear:
                return vk::ImageTiling::eLinear;
            default:
                throw std::invalid_argument("Invalid GPUImageTiling");
        }
    }

    constexpr vk::ImageAspectFlagBits toVkImageAspectFlag(const GPUImageAspect aspect) {
        switch (aspect) {
            case GPUImageAspect::Color:
                return vk::ImageAspectFlagBits::eColor;
            case GPUImageAspect::Depth:
                return vk::ImageAspectFlagBits::eDepth;
            case GPUImageAspect::Stencil:
                return vk::ImageAspectFlagBits::eStencil;
            case GPUImageAspect::Metadata:
                return vk::ImageAspectFlagBits::eMetadata;
            default:
                throw std::invalid_argument("Invalid GPUImageAspect");
        }
    }

    constexpr vk::ImageAspectFlags toVkImageAspectFlags(const GPUImageAspectFlags aspects) {
        vk::ImageAspectFlags flags = {};
        if (aspects.contains(GPUImageAspect::Color)) {
            flags |= vk::ImageAspectFlagBits::eColor;
        }
        if (aspects.contains(GPUImageAspect::Depth)) {
            flags |= vk::ImageAspectFlagBits::eDepth;
        }
        if (aspects.contains(GPUImageAspect::Stencil)) {
            flags |= vk::ImageAspectFlagBits::eStencil;
        }
        if (aspects.contains(GPUImageAspect::Metadata)) {
            flags |= vk::ImageAspectFlagBits::eMetadata;
        }
        return flags;
    }

    constexpr RenderSurfaceFormat toSurfaceFormat(const vk::SurfaceFormatKHR& format) {
        return {toFormat(format.format), toColorSpace(format.colorSpace)};
    }

    constexpr vk::SurfaceFormatKHR toVkSurfaceFormat(const RenderSurfaceFormat& format) {
        return vk::SurfaceFormatKHR(toVkFormat(format.imageFormat),
                                    toVkColorSpace(format.colorSpace));
    }

    // --- Sampler ---

    constexpr vk::SamplerMipmapMode toVkSamplerMipmapMode(const SamplerMipmapMode mode) {
        switch (mode) {
            case SamplerMipmapMode::Nearest:
                return vk::SamplerMipmapMode::eNearest;
            case SamplerMipmapMode::Linear:
                return vk::SamplerMipmapMode::eLinear;
            default:
                throw std::invalid_argument("Invalid SamplerMipmapMode");
        }
    }

    constexpr vk::SamplerAddressMode toVkSamplerAddressMode(const SamplerAddressMode mode) {
        switch (mode) {
            case SamplerAddressMode::Repeat:
                return vk::SamplerAddressMode::eRepeat;
            case SamplerAddressMode::MirroredRepeat:
                return vk::SamplerAddressMode::eMirroredRepeat;
            case SamplerAddressMode::ClampToEdge:
                return vk::SamplerAddressMode::eClampToEdge;
            case SamplerAddressMode::ClampToBorder:
                return vk::SamplerAddressMode::eClampToBorder;
            case SamplerAddressMode::MirrorClampToEdge:
                return vk::SamplerAddressMode::eMirrorClampToEdge;
            default:
                throw std::invalid_argument("Invalid SamplerAddressMode");
        }
    }

    constexpr vk::BorderColor toVkBorderColor(const SamplerBorderColor color) {
        switch (color) {
            case SamplerBorderColor::FloatTransparentBlack:
                return vk::BorderColor::eFloatTransparentBlack;
            case SamplerBorderColor::IntTransparentBlack:
                return vk::BorderColor::eIntTransparentBlack;
            case SamplerBorderColor::FloatOpaqueBlack:
                return vk::BorderColor::eFloatOpaqueBlack;
            case SamplerBorderColor::IntOpaqueBlack:
                return vk::BorderColor::eIntOpaqueBlack;
            case SamplerBorderColor::FloatOpaqueWhite:
                return vk::BorderColor::eFloatOpaqueWhite;
            case SamplerBorderColor::IntOpaqueWhite:
                return vk::BorderColor::eIntOpaqueWhite;
            default:
                throw std::invalid_argument("Invalid SamplerBorderColor");
        }
    }

    constexpr vk::Filter toVkFilter(const FilterMode filter) {
        switch (filter) {
            case FilterMode::Nearest:
                return vk::Filter::eNearest;
            case FilterMode::Linear:
                return vk::Filter::eLinear;
            default:
                throw std::invalid_argument("Invalid FilterMode");
        }
    }

    constexpr vk::SamplerAddressMode toVkSamplerAddressMode(const AddressMode mode) {
        switch (mode) {
            case AddressMode::Repeat:
                return vk::SamplerAddressMode::eRepeat;
            case AddressMode::MirroredRepeat:
                return vk::SamplerAddressMode::eMirroredRepeat;
            case AddressMode::ClampToEdge:
                return vk::SamplerAddressMode::eClampToEdge;
            case AddressMode::ClampToBorder:
                return vk::SamplerAddressMode::eClampToBorder;
            case AddressMode::MirrorClampToEdge:
                return vk::SamplerAddressMode::eMirrorClampToEdge;
            default:
                throw std::invalid_argument("Invalid AddressMode");
        }
    }

    constexpr vk::ImageSubresourceRange toVkImageSubresourceRange(
        const GPUImageSubresourceDescription& range) {
        return vk::ImageSubresourceRange(toVkImageAspectFlags(range.aspectMask),
                                         range.range.baseMipLevel, range.range.layerCount,
                                         range.range.baseArrayLayer, range.range.layerCount);
    }

    // --- Render Pass / Attachment ---

    constexpr vk::AttachmentLoadOp toVkAttachmentLoadOp(const AttachmentLoadOp op) {
        switch (op) {
            case AttachmentLoadOp::Load:
                return vk::AttachmentLoadOp::eLoad;
            case AttachmentLoadOp::Clear:
                return vk::AttachmentLoadOp::eClear;
            case AttachmentLoadOp::DontCare:
                return vk::AttachmentLoadOp::eDontCare;
            default:
                throw std::invalid_argument("Invalid AttachmentLoadOp");
        }
    }

    constexpr vk::AttachmentStoreOp toVkAttachmentStoreOp(const AttachmentStoreOp op) {
        switch (op) {
            case AttachmentStoreOp::Store:
                return vk::AttachmentStoreOp::eStore;
            case AttachmentStoreOp::DontCare:
                return vk::AttachmentStoreOp::eDontCare;
            default:
                throw std::invalid_argument("Invalid AttachmentStoreOp");
        }
    }

    constexpr vk::ResolveModeFlagBits toVkResolveMode(const ResolveMode mode) {
        switch (mode) {
            case ResolveMode::None:
                return {};
            case ResolveMode::SampleZero:
                return vk::ResolveModeFlagBits::eSampleZero;
            case ResolveMode::Average:
                return vk::ResolveModeFlagBits::eAverage;
            case ResolveMode::Min:
                return vk::ResolveModeFlagBits::eMin;
            case ResolveMode::Max:
                return vk::ResolveModeFlagBits::eMax;
            default:
                throw std::invalid_argument("Invalid ResolveMode");
        }
    }

    constexpr vk::ResolveModeFlags toVkResolveModes(const ResolveModeFlags modes) {
        vk::ResolveModeFlags flags = {};
        if (modes.contains(ResolveMode::None)) {
            flags |= {};
        }
        if (modes.contains(ResolveMode::SampleZero)) {
            flags |= vk::ResolveModeFlagBits::eSampleZero;
        }
        if (modes.contains(ResolveMode::Average)) {
            flags |= vk::ResolveModeFlagBits::eAverage;
        }
        if (modes.contains(ResolveMode::Min)) {
            flags |= vk::ResolveModeFlagBits::eMin;
        }
        if (modes.contains(ResolveMode::Max)) {
            flags |= vk::ResolveModeFlagBits::eMax;
        }
        return flags;
    }

    // --- Blending ---

    constexpr vk::BlendFactor toVkBlendFactor(const BlendFactor factor) {
        switch (factor) {
            case BlendFactor::Zero:
                return vk::BlendFactor::eZero;
            case BlendFactor::One:
                return vk::BlendFactor::eOne;
            case BlendFactor::SrcColor:
                return vk::BlendFactor::eSrcColor;
            case BlendFactor::OneMinusSrcColor:
                return vk::BlendFactor::eOneMinusSrcColor;
            case BlendFactor::DstColor:
                return vk::BlendFactor::eDstColor;
            case BlendFactor::OneMinusDstColor:
                return vk::BlendFactor::eOneMinusDstColor;
            case BlendFactor::SrcAlpha:
                return vk::BlendFactor::eSrcAlpha;
            case BlendFactor::OneMinusSrcAlpha:
                return vk::BlendFactor::eOneMinusSrcAlpha;
            case BlendFactor::DstAlpha:
                return vk::BlendFactor::eDstAlpha;
            case BlendFactor::OneMinusDstAlpha:
                return vk::BlendFactor::eOneMinusDstAlpha;
            case BlendFactor::ConstantColor:
                return vk::BlendFactor::eConstantColor;
            case BlendFactor::OneMinusConstantColor:
                return vk::BlendFactor::eOneMinusConstantColor;
            case BlendFactor::ConstantAlpha:
                return vk::BlendFactor::eConstantAlpha;
            case BlendFactor::OneMinusConstantAlpha:
                return vk::BlendFactor::eOneMinusConstantAlpha;
            default:
                throw std::invalid_argument("Invalid BlendFactor");
        }
    }

    constexpr vk::BlendOp toVkBlendOp(const BlendOp op) {
        switch (op) {
            case BlendOp::Add:
                return vk::BlendOp::eAdd;
            case BlendOp::Subtract:
                return vk::BlendOp::eSubtract;
            case BlendOp::ReverseSubtract:
                return vk::BlendOp::eReverseSubtract;
            case BlendOp::Min:
                return vk::BlendOp::eMin;
            case BlendOp::Max:
                return vk::BlendOp::eMax;
            default:
                throw std::invalid_argument("Invalid BlendOp");
        }
    }

    constexpr vk::ColorComponentFlagBits toVkColorComponentFlag(const ColorComponent component) {
        switch (component) {
            case ColorComponent::None:
                return {};
            case ColorComponent::R:
                return vk::ColorComponentFlagBits::eR;
            case ColorComponent::G:
                return vk::ColorComponentFlagBits::eG;
            case ColorComponent::B:
                return vk::ColorComponentFlagBits::eB;
            case ColorComponent::A:
                return vk::ColorComponentFlagBits::eA;
            default:
                throw std::invalid_argument("Invalid ColorComponent");
        }
    }

    constexpr vk::ColorComponentFlags toVkColorComponentFlags(
        const ColorComponentFlags components) {
        vk::ColorComponentFlags flags = {};
        if (components.contains(ColorComponent::R)) {
            flags |= vk::ColorComponentFlagBits::eR;
        }
        if (components.contains(ColorComponent::G)) {
            flags |= vk::ColorComponentFlagBits::eG;
        }
        if (components.contains(ColorComponent::B)) {
            flags |= vk::ColorComponentFlagBits::eB;
        }
        if (components.contains(ColorComponent::A)) {
            flags |= vk::ColorComponentFlagBits::eA;
        }
        return flags;
    }

    constexpr vk::LogicOp toVkLogicOp(const BlendingLogicOp op) {
        switch (op) {
            case BlendingLogicOp::Clear:
                return vk::LogicOp::eClear;
            case BlendingLogicOp::And:
                return vk::LogicOp::eAnd;
            case BlendingLogicOp::AndReverse:
                return vk::LogicOp::eAndReverse;
            case BlendingLogicOp::Copy:
                return vk::LogicOp::eCopy;
            case BlendingLogicOp::AndInverted:
                return vk::LogicOp::eAndInverted;
            case BlendingLogicOp::NoOp:
                return vk::LogicOp::eNoOp;
            case BlendingLogicOp::Xor:
                return vk::LogicOp::eXor;
            case BlendingLogicOp::Or:
                return vk::LogicOp::eOr;
            case BlendingLogicOp::Nor:
                return vk::LogicOp::eNor;
            case BlendingLogicOp::Equivalent:
                return vk::LogicOp::eEquivalent;
            case BlendingLogicOp::Invert:
                return vk::LogicOp::eInvert;
            case BlendingLogicOp::OrReverse:
                return vk::LogicOp::eOrReverse;
            case BlendingLogicOp::CopyInverted:
                return vk::LogicOp::eCopyInverted;
            case BlendingLogicOp::OrInverted:
                return vk::LogicOp::eOrInverted;
            case BlendingLogicOp::Nand:
                return vk::LogicOp::eNand;
            case BlendingLogicOp::Set:
                return vk::LogicOp::eSet;
            default:
                throw std::invalid_argument("Invalid BlendingLogicOp");
        }
    }

    // --- Rasterization ---

    constexpr vk::PrimitiveTopology toVkPrimitiveTopology(const PrimitiveTopology topology) {
        switch (topology) {
            case PrimitiveTopology::PointList:
                return vk::PrimitiveTopology::ePointList;
            case PrimitiveTopology::LineList:
                return vk::PrimitiveTopology::eLineList;
            case PrimitiveTopology::LineStrip:
                return vk::PrimitiveTopology::eLineStrip;
            case PrimitiveTopology::TriangleList:
                return vk::PrimitiveTopology::eTriangleList;
            case PrimitiveTopology::TriangleStrip:
                return vk::PrimitiveTopology::eTriangleStrip;
            case PrimitiveTopology::TriangleFan:
                return vk::PrimitiveTopology::eTriangleFan;
            default:
                throw std::invalid_argument("Invalid PrimitiveTopology");
        }
    }

    constexpr vk::PolygonMode toVkPolygonMode(const PolygonMode mode) {
        switch (mode) {
            case PolygonMode::Fill:
                return vk::PolygonMode::eFill;
            case PolygonMode::Line:
                return vk::PolygonMode::eLine;
            case PolygonMode::Point:
                return vk::PolygonMode::ePoint;
            default:
                throw std::invalid_argument("Invalid PolygonMode");
        }
    }

    constexpr vk::CullModeFlagBits toVkCullMode(const CullMode mode) {
        switch (mode) {
            case CullMode::None:
                return {};
            case CullMode::Front:
                return vk::CullModeFlagBits::eFront;
            case CullMode::Back:
                return vk::CullModeFlagBits::eBack;
            case CullMode::FrontAndBack:
                return vk::CullModeFlagBits::eFrontAndBack;
            default:
                throw std::invalid_argument("Invalid CullMode");
        }
    }

    constexpr vk::FrontFace toVkFrontFace(const FrontFace face) {
        switch (face) {
            case FrontFace::CounterClockwise:
                return vk::FrontFace::eCounterClockwise;
            case FrontFace::Clockwise:
                return vk::FrontFace::eClockwise;
            default:
                throw std::invalid_argument("Invalid FrontFace");
        }
    }

    // --- Depth/Stencil ---

    constexpr vk::CompareOp toVkCompareOp(const CompareOp op) {
        switch (op) {
            case CompareOp::Never:
                return vk::CompareOp::eNever;
            case CompareOp::Less:
                return vk::CompareOp::eLess;
            case CompareOp::Equal:
                return vk::CompareOp::eEqual;
            case CompareOp::LessOrEqual:
                return vk::CompareOp::eLessOrEqual;
            case CompareOp::Greater:
                return vk::CompareOp::eGreater;
            case CompareOp::NotEqual:
                return vk::CompareOp::eNotEqual;
            case CompareOp::GreaterOrEqual:
                return vk::CompareOp::eGreaterOrEqual;
            case CompareOp::Always:
                return vk::CompareOp::eAlways;
            default:
                throw std::invalid_argument("Invalid CompareOp");
        }
    }

    constexpr vk::StencilOp toVkStencilOp(const StencilOp op) {
        switch (op) {
            case StencilOp::Keep:
                return vk::StencilOp::eKeep;
            case StencilOp::Zero:
                return vk::StencilOp::eZero;
            case StencilOp::Replace:
                return vk::StencilOp::eReplace;
            case StencilOp::IncrementAndClamp:
                return vk::StencilOp::eIncrementAndClamp;
            case StencilOp::DecrementAndClamp:
                return vk::StencilOp::eDecrementAndClamp;
            case StencilOp::Invert:
                return vk::StencilOp::eInvert;
            case StencilOp::IncrementAndWrap:
                return vk::StencilOp::eIncrementAndWrap;
            case StencilOp::DecrementAndWrap:
                return vk::StencilOp::eDecrementAndWrap;
            default:
                throw std::invalid_argument("Invalid StencilOp");
        }
    }

    // --- Vertex description ---

    constexpr vk::VertexInputRate toVkVertexInputRate(const VertexInputRate rate) {
        switch (rate) {
            case VertexInputRate::Vertex:
                return vk::VertexInputRate::eVertex;
            case VertexInputRate::Instance:
                return vk::VertexInputRate::eInstance;
            default:
                throw std::invalid_argument("Invalid VertexInputRate");
        }
    }

    constexpr vk::Format toVkVertexAttributeFormat(const VertexAttributeFormat format) {
        switch (format) {
            case VertexAttributeFormat::Float:
                return vk::Format::eR32Sfloat;
            case VertexAttributeFormat::Float2:
                return vk::Format::eR32G32Sfloat;
            case VertexAttributeFormat::Float3:
                return vk::Format::eR32G32B32Sfloat;
            case VertexAttributeFormat::Float4:
                return vk::Format::eR32G32B32A32Sfloat;
            case VertexAttributeFormat::Int:
                return vk::Format::eR32Sint;
            case VertexAttributeFormat::Int2:
                return vk::Format::eR32G32Sint;
            case VertexAttributeFormat::Int3:
                return vk::Format::eR32G32B32Sint;
            case VertexAttributeFormat::Int4:
                return vk::Format::eR32G32B32A32Sint;
            case VertexAttributeFormat::UInt:
                return vk::Format::eR32Uint;
            case VertexAttributeFormat::UInt2:
                return vk::Format::eR32G32Uint;
            case VertexAttributeFormat::UInt3:
                return vk::Format::eR32G32B32Uint;
            case VertexAttributeFormat::UInt4:
                return vk::Format::eR32G32B32A32Uint;
            case VertexAttributeFormat::Mat2:
                return vk::Format::eR32G32Sfloat;  // Treated as array of vec2
            case VertexAttributeFormat::Mat3:
                return vk::Format::eR32G32B32Sfloat;  // Treated as array of vec3
            case VertexAttributeFormat::Mat4:
                return vk::Format::eR32G32B32A32Sfloat;  // Treated as array of vec4
            default:
                throw std::invalid_argument("Invalid VertexAttributeFormat");
        }
    }

    constexpr vk::IndexType toVkIndexType(const IndexType type) {
        switch (type) {
            case IndexType::UInt16:
                return vk::IndexType::eUint16;
            case IndexType::UInt32:
                return vk::IndexType::eUint32;
            default:
                throw std::invalid_argument("Invalid IndexType");
        }
    }

    // --- Multisampling ---

    constexpr vk::SampleCountFlagBits toVkSampleCount(const SampleCount count) {
        switch (count) {
            case SampleCount::Count1:
                return vk::SampleCountFlagBits::e1;
            case SampleCount::Count2:
                return vk::SampleCountFlagBits::e2;
            case SampleCount::Count4:
                return vk::SampleCountFlagBits::e4;
            case SampleCount::Count8:
                return vk::SampleCountFlagBits::e8;
            case SampleCount::Count16:
                return vk::SampleCountFlagBits::e16;
            default:
                throw std::invalid_argument("Invalid SampleCount");
        }
    }

}  // namespace aetherion