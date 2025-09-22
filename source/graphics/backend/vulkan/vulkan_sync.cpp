#include "vulkan_sync.hpp"

#include <fmt/core.h>

#include "vulkan_device.hpp"

namespace aetherion {
    VulkanFence::VulkanFence(VulkanDevice& device, const GPUFenceDescription& description)
        : device_(device.getVkDevice()) {
        vk::FenceCreateInfo fenceCreateInfo;
        if (description.signaled) {
            fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        }

        fence_ = device_.createFence(fenceCreateInfo);
    }

    VulkanFence::VulkanFence(vk::Device device, vk::Fence fence) : device_(device), fence_(fence) {}

    VulkanFence::~VulkanFence() noexcept { clear(); }

    VulkanFence::VulkanFence(VulkanFence&& other) noexcept
        : IGPUFence(std::move(other)), device_(other.device_), fence_(other.fence_) {
        other.fence_ = nullptr;
        other.device_ = nullptr;
    }

    VulkanFence& VulkanFence::operator=(VulkanFence&& other) noexcept {
        if (this != &other) {
            clear();

            IGPUFence::operator=(std::move(other));
            device_ = other.device_;
            fence_ = other.fence_;

            other.release();
        }
        return *this;
    }

    void VulkanFence::clear() noexcept {
        if (fence_ && device_) {
            device_.destroyFence(fence_);
            fence_ = nullptr;
        }
        device_ = nullptr;
    }

    void VulkanFence::release() noexcept {
        fence_ = nullptr;
        device_ = nullptr;
    }

    void VulkanFence::wait(uint64_t timeout) {
        auto result = device_.waitForFences(1, &fence_, VK_TRUE, timeout);

        if (result == vk::Result::eTimeout) {
            throw(std::runtime_error("Timeout while waiting for fence."));
        } else if (result != vk::Result::eSuccess) {
            throw(std::runtime_error(
                fmt::format("Failed to wait for fence. Error: {}", vk::to_string(result))));
        }
    }

    void VulkanFence::reset() {
        auto result = device_.resetFences(1, &fence_);

        if (result != vk::Result::eSuccess) {
            throw(std::runtime_error(
                fmt::format("Failed to reset fence. Error: {}", vk::to_string(result))));
        }
    }

    bool VulkanFence::isSignaled() const {
        auto result = device_.getFenceStatus(fence_);

        if (result == vk::Result::eSuccess) {
            return true;
        } else if (result == vk::Result::eNotReady) {
            return false;
        } else {
            throw(std::runtime_error(
                fmt::format("Failed to get fence status. Error: {}", vk::to_string(result))));
        }
    }

    VulkanBinarySemaphore::VulkanBinarySemaphore(VulkanDevice& device,
                                                 const GPUBinarySemaphoreDescription& description)
        : device_(device.getVkDevice()) {
        semaphore_ = device_.createSemaphore({});
    }

    VulkanBinarySemaphore::VulkanBinarySemaphore(vk::Device device, vk::Semaphore semaphore)
        : device_(device), semaphore_(semaphore) {}

    VulkanBinarySemaphore::~VulkanBinarySemaphore() noexcept { clear(); }

    VulkanBinarySemaphore::VulkanBinarySemaphore(VulkanBinarySemaphore&& other) noexcept
        : IGPUBinarySemaphore(std::move(other)),
          device_(other.device_),
          semaphore_(other.semaphore_) {
        other.semaphore_ = nullptr;
        other.device_ = nullptr;
    }

    VulkanBinarySemaphore& VulkanBinarySemaphore::operator=(
        VulkanBinarySemaphore&& other) noexcept {
        if (this != &other) {
            clear();

            IGPUBinarySemaphore::operator=(std::move(other));
            device_ = other.device_;
            semaphore_ = other.semaphore_;

            other.release();
        }
        return *this;
    }

    void VulkanBinarySemaphore::clear() noexcept {
        if (semaphore_ && device_) {
            device_.destroySemaphore(semaphore_);
            semaphore_ = nullptr;
        }
    }

    void VulkanBinarySemaphore::release() noexcept {
        semaphore_ = nullptr;
        device_ = nullptr;
    }

    VulkanTimelineSemaphore::VulkanTimelineSemaphore(
        VulkanDevice& device, const GPUTimelineSemaphoreDescription& description)
        : device_(device.getVkDevice()) {
        auto semaphoreTypeCreateInfo = vk::SemaphoreTypeCreateInfo()
                                           .setSemaphoreType(vk::SemaphoreType::eTimeline)
                                           .setInitialValue(description.initialValue);

        auto semaphoreCreateInfo = vk::SemaphoreCreateInfo().setPNext(&semaphoreTypeCreateInfo);

        semaphore_ = device_.createSemaphore(semaphoreCreateInfo);
    }

    VulkanTimelineSemaphore::VulkanTimelineSemaphore(vk::Device device, vk::Semaphore semaphore)
        : device_(device), semaphore_(semaphore) {}

    VulkanTimelineSemaphore::~VulkanTimelineSemaphore() noexcept { clear(); }

    VulkanTimelineSemaphore::VulkanTimelineSemaphore(VulkanTimelineSemaphore&& other) noexcept
        : IGPUTimelineSemaphore(std::move(other)),
          device_(other.device_),
          semaphore_(other.semaphore_) {
        other.semaphore_ = nullptr;
        other.device_ = nullptr;
    }

    VulkanTimelineSemaphore& VulkanTimelineSemaphore::operator=(
        VulkanTimelineSemaphore&& other) noexcept {
        if (this != &other) {
            clear();

            IGPUTimelineSemaphore::operator=(std::move(other));
            device_ = other.device_;
            semaphore_ = other.semaphore_;

            other.release();
        }
        return *this;
    }

    void VulkanTimelineSemaphore::clear() noexcept {
        if (semaphore_ && device_) {
            device_.destroySemaphore(semaphore_);
            semaphore_ = nullptr;
        }
    }

    void VulkanTimelineSemaphore::release() noexcept {
        semaphore_ = nullptr;
        device_ = nullptr;
    }

    void VulkanTimelineSemaphore::wait(uint64_t value, uint64_t timeout) {
        auto waitInfo = vk::SemaphoreWaitInfo().setSemaphores(semaphore_).setValues(value);

        auto result = device_.waitSemaphores(waitInfo, timeout);

        if (result == vk::Result::eTimeout) {
            throw(std::runtime_error("Timeout while waiting for timeline semaphore."));
        } else if (result != vk::Result::eSuccess) {
            throw(std::runtime_error(fmt::format("Failed to wait for timeline semaphore. Error: {}",
                                                 vk::to_string(result))));
        }
    }

    void VulkanTimelineSemaphore::signal(uint64_t value) {
        device_.signalSemaphore(vk::SemaphoreSignalInfo().setSemaphore(semaphore_).setValue(value));
    }

    uint64_t VulkanTimelineSemaphore::getCurrentValue() const {
        return device_.getSemaphoreCounterValue(semaphore_);
    }
}  // namespace aetherion