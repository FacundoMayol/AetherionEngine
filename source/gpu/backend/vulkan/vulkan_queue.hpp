#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/gpu/backend/queue.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanQueue : public IGPUQueue {
      public:
        VulkanQueue() = delete;
        VulkanQueue(VulkanDevice& device, const GPUQueueDescription& description);
        VulkanQueue(vk::Device device, vk::Queue queue);
        ~VulkanQueue() noexcept override;

        VulkanQueue(const VulkanQueue&) = delete;
        VulkanQueue& operator=(const VulkanQueue&) = delete;

        VulkanQueue(VulkanQueue&&) noexcept;
        VulkanQueue& operator=(VulkanQueue&&) noexcept;

        void submit(std::span<GPUQueueSubmitDescription> submitDescriptions,
                    IGPUFence* fence) override;

        std::pair<QueuePresentResultCode, std::vector<QueuePresentResultCode>> present(
            const GPUQueuePresentDescription& presentDescription) override;

        void waitIdle() override;

        inline vk::Queue getVkQueue() const { return queue_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;

        vk::Queue queue_;
    };
}  // namespace aetherion