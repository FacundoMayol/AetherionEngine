#pragma once

#include <optional>
#include <span>
#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"

namespace aetherion {
    // Forward declarations
    class ICommandBuffer;
    class ISwapchain;
    class IGPUFence;
    class IGPUBinarySemaphore;
    class IGPUTimelineSemaphore;

    struct QueueDescription {
        uint32_t familyIndex;
        uint32_t index;
    };

    struct QueueSubmitDescription {
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

    struct QueuePresentDescription {
        std::vector<IGPUBinarySemaphore*> waitSemaphores;
        std::vector<std::pair<ISwapchain*, uint32_t>> swapchains;
    };

    class IQueue {
      public:
        virtual ~IQueue() = 0;

        IQueue(const IQueue&) = delete;
        IQueue& operator=(const IQueue&) = delete;

        virtual void submit(std::span<QueueSubmitDescription> submitDescriptions, IGPUFence* fence)
            = 0;

        virtual std::pair<QueuePresentResultCode, std::vector<QueuePresentResultCode>> present(
            const QueuePresentDescription& presentDescription)
            = 0;

        virtual void waitIdle() = 0;

      protected:
        IQueue() = default;
        IQueue(IQueue&&) noexcept = default;
        IQueue& operator=(IQueue&&) noexcept = default;
    };
}  // namespace aetherion
