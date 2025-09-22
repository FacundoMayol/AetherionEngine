#pragma once

#include <optional>
#include <span>
#include <vector>

#include "aetherion/gpu/backend/render_definitions.hpp"

namespace aetherion {
    // Forward declarations
    class ICommandBuffer;
    class ISwapchain;
    class IGPUFence;
    class IGPUBinarySemaphore;
    class IGPUTimelineSemaphore;

    struct GPUQueueDescription {
        uint32_t familyIndex;
        uint32_t index;
    };

    struct GPUQueueSubmitDescription {
        struct WaitBinarySemaphoreInfo {
            IGPUBinarySemaphore* semaphore;
            PipelineStageFlags waitStage = {};
        };
        std::vector<WaitBinarySemaphoreInfo> waitBinarySemaphores;
        struct WaitTimelineSemaphoreInfo {
            IGPUTimelineSemaphore* semaphore;
            uint64_t value;
            PipelineStageFlags waitStage = {};
        };
        std::vector<WaitTimelineSemaphoreInfo> waitTimelineSemaphores;

        std::vector<ICommandBuffer*> commandBuffers;

        struct SignalBinarySemaphoreInfo {
            IGPUBinarySemaphore* semaphore;
            PipelineStageFlags signalStage = {};
        };
        std::vector<SignalBinarySemaphoreInfo> signalBinarySemaphores;
        struct SignalTimelineSemaphoreInfo {
            IGPUTimelineSemaphore* semaphore;
            uint64_t value;
            PipelineStageFlags signalStage = {};
        };
        std::vector<SignalTimelineSemaphoreInfo> signalTimelineSemaphores;

        std::optional<IGPUFence*> fence;
    };

    struct GPUQueuePresentDescription {
        std::vector<IGPUBinarySemaphore*> waitSemaphores;
        std::vector<std::pair<ISwapchain*, uint32_t>> swapchains;
    };

    class IGPUQueue {
      public:
        virtual ~IGPUQueue() = 0;

        IGPUQueue(const IGPUQueue&) = delete;
        IGPUQueue& operator=(const IGPUQueue&) = delete;

        virtual void submit(std::span<GPUQueueSubmitDescription> submitDescriptions,
                            IGPUFence* fence)
            = 0;

        virtual std::pair<QueuePresentResultCode, std::vector<QueuePresentResultCode>> present(
            const GPUQueuePresentDescription& presentDescription)
            = 0;

        virtual void waitIdle() = 0;

      protected:
        IGPUQueue() = default;
        IGPUQueue(IGPUQueue&&) noexcept = default;
        IGPUQueue& operator=(IGPUQueue&&) noexcept = default;
    };
}  // namespace aetherion
