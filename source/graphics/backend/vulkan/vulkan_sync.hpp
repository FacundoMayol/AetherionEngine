#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/sync.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;

    class VulkanFence : public IGPUFence {
      public:
        VulkanFence() = delete;
        VulkanFence(VulkanDevice& device, const GPUFenceDescription& description);
        VulkanFence(VulkanDevice& device, vk::Fence fence);
        virtual ~VulkanFence() noexcept override;

        VulkanFence(const VulkanFence&) = delete;
        VulkanFence& operator=(const VulkanFence&) = delete;

        VulkanFence(VulkanFence&&) noexcept;
        VulkanFence& operator=(VulkanFence&&) noexcept;

        virtual void wait(uint64_t timeout) override;
        virtual void reset() override;
        virtual bool isSignaled() const override;

        inline vk::Fence getVkFence() const { return fence_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;

        vk::Fence fence_;
    };

    class VulkanBinarySemaphore : public IGPUBinarySemaphore {
      public:
        VulkanBinarySemaphore() = delete;
        VulkanBinarySemaphore(VulkanDevice& device,
                              const GPUBinarySemaphoreDescription& description);
        VulkanBinarySemaphore(VulkanDevice& device, vk::Semaphore semaphore);
        virtual ~VulkanBinarySemaphore() noexcept override;

        VulkanBinarySemaphore(const VulkanBinarySemaphore&) = delete;
        VulkanBinarySemaphore& operator=(const VulkanBinarySemaphore&) = delete;

        VulkanBinarySemaphore(VulkanBinarySemaphore&&) noexcept;
        VulkanBinarySemaphore& operator=(VulkanBinarySemaphore&&) noexcept;

        inline vk::Semaphore getVkSemaphore() const { return semaphore_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;

        vk::Semaphore semaphore_;
    };

    class VulkanTimelineSemaphore : public IGPUTimelineSemaphore {
      public:
        VulkanTimelineSemaphore() = delete;
        VulkanTimelineSemaphore(VulkanDevice& device,
                                const GPUTimelineSemaphoreDescription& description);
        VulkanTimelineSemaphore(VulkanDevice& device, vk::Semaphore semaphore);
        virtual ~VulkanTimelineSemaphore() noexcept override;

        VulkanTimelineSemaphore(const VulkanTimelineSemaphore&) = delete;
        VulkanTimelineSemaphore& operator=(const VulkanTimelineSemaphore&) = delete;

        VulkanTimelineSemaphore(VulkanTimelineSemaphore&&) noexcept;
        VulkanTimelineSemaphore& operator=(VulkanTimelineSemaphore&&) noexcept;

        virtual void wait(uint64_t value, uint64_t timeout) override;
        virtual void signal(uint64_t value) override;
        virtual uint64_t getCurrentValue() const override;

        inline vk::Semaphore getVkSemaphore() const { return semaphore_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        VulkanDevice* device_;

        vk::Semaphore semaphore_;
    };
}  // namespace aetherion