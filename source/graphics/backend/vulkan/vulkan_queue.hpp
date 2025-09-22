#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/queue.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanQueue : public IQueue {
      public:
        VulkanQueue() = delete;
        VulkanQueue(VulkanDevice& device, const QueueDescription& description);
        VulkanQueue(vk::Device device, vk::Queue queue);
        virtual ~VulkanQueue() noexcept override;

        VulkanQueue(const VulkanQueue&) = delete;
        VulkanQueue& operator=(const VulkanQueue&) = delete;

        VulkanQueue(VulkanQueue&&) noexcept;
        VulkanQueue& operator=(VulkanQueue&&) noexcept;

        virtual void submit(std::span<QueueSubmitDescription> submitDescriptions,
                            IGPUFence* fence) override;

        virtual std::pair<QueuePresentResultCode, std::vector<QueuePresentResultCode>> present(
            const QueuePresentDescription& presentDescription) override;

        virtual void waitIdle() override;

        inline vk::Queue getVkQueue() const { return queue_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;

        vk::Queue queue_;
    };
}  // namespace aetherion