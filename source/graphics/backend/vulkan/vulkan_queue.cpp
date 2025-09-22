#include "vulkan_queue.hpp"

#include <fmt/core.h>

#include "vulkan_command_buffer.hpp"
#include "vulkan_device.hpp"
#include "vulkan_render_definitions.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_sync.hpp"

namespace aetherion {
    VulkanQueue::VulkanQueue(VulkanDevice& device, const QueueDescription& description)
        : device_(device.getVkDevice()) {
        queue_ = device_.getQueue(description.familyIndex, description.index);
    }

    VulkanQueue::VulkanQueue(vk::Device device, vk::Queue queue) : device_(device), queue_(queue) {}

    VulkanQueue::~VulkanQueue() noexcept { clear(); }

    VulkanQueue::VulkanQueue(VulkanQueue&& other) noexcept
        : IQueue(std::move(other)), device_(other.device_), queue_(other.queue_) {
        other.device_ = nullptr;
        other.queue_ = nullptr;
    }

    VulkanQueue& VulkanQueue::operator=(VulkanQueue&& other) noexcept {
        if (this != &other) {
            clear();

            IQueue::operator=(std::move(other));
            device_ = other.device_;
            queue_ = other.queue_;

            other.release();
        }
        return *this;
    }

    void VulkanQueue::clear() noexcept {
        // NOTE: Queues are not owned, so we don't destroy them.
        release();
    }

    void VulkanQueue::release() noexcept {
        device_ = nullptr;
        queue_ = nullptr;
    }

    void VulkanQueue::submit(std::span<QueueSubmitDescription> submitDescriptions,
                             IGPUFence* fence) {
        // NOTE: It's valid to pass nullptr as fence.
        auto* vkFence = dynamic_cast<VulkanFence*>(fence);

        std::vector<vk::SubmitInfo2> vkSubmitInfos;
        vkSubmitInfos.reserve(submitDescriptions.size());
        for (const auto& submitDescription : submitDescriptions) {
            std::vector<vk::SemaphoreSubmitInfo> vkWaitSemaphoreInfos;
            vkWaitSemaphoreInfos.reserve(submitDescription.waitBinarySemaphores.size()
                                         + submitDescription.waitTimelineSemaphores.size());
            std::vector<vk::CommandBufferSubmitInfo> vkCommandBufferInfos;
            vkCommandBufferInfos.reserve(submitDescription.commandBuffers.size());
            std::vector<vk::SemaphoreSubmitInfo> vkSignalSemaphoreInfos;
            vkSignalSemaphoreInfos.reserve(submitDescription.signalBinarySemaphores.size()
                                           + submitDescription.signalTimelineSemaphores.size());

            for (const auto& waitBinarySemaphoreInfo : submitDescription.waitBinarySemaphores) {
                if (!waitBinarySemaphoreInfo.semaphore) {
                    throw std::invalid_argument("Binary semaphore pointer is null");
                }
                auto& vkWaitSemaphore
                    = dynamic_cast<VulkanBinarySemaphore&>(*waitBinarySemaphoreInfo.semaphore);

                vkWaitSemaphoreInfos.push_back(
                    vk::SemaphoreSubmitInfo()
                        .setSemaphore(vkWaitSemaphore.getVkSemaphore())
                        .setStageMask(toVkPipelineStageFlags(waitBinarySemaphoreInfo.waitStage)));
            }
            for (const auto& waitTimelineSemaphoreInfo : submitDescription.waitTimelineSemaphores) {
                if (!waitTimelineSemaphoreInfo.semaphore) {
                    throw std::invalid_argument("Timeline semaphore pointer is null");
                }
                auto& vkWaitSemaphore
                    = dynamic_cast<VulkanTimelineSemaphore&>(*waitTimelineSemaphoreInfo.semaphore);

                vkWaitSemaphoreInfos.push_back(
                    vk::SemaphoreSubmitInfo()
                        .setSemaphore(vkWaitSemaphore.getVkSemaphore())
                        .setStageMask(toVkPipelineStageFlags(waitTimelineSemaphoreInfo.waitStage))
                        .setValue(waitTimelineSemaphoreInfo.value));
            }
            for (const auto& commandBuffer : submitDescription.commandBuffers) {
                if (!commandBuffer) {
                    throw std::invalid_argument("Command buffer pointer is null");
                }
                auto& vkCommandBuffer = dynamic_cast<VulkanCommandBuffer&>(*commandBuffer);

                vkCommandBufferInfos.push_back(vk::CommandBufferSubmitInfo().setCommandBuffer(
                    vkCommandBuffer.getVkCommandBuffer()));
            }
            for (const auto& signalBinarySemaphoreInfo : submitDescription.signalBinarySemaphores) {
                if (!signalBinarySemaphoreInfo.semaphore) {
                    throw std::invalid_argument("Binary semaphore pointer is null");
                }
                auto& vkSignalSemaphore
                    = dynamic_cast<VulkanBinarySemaphore&>(*signalBinarySemaphoreInfo.semaphore);

                vkSignalSemaphoreInfos.push_back(
                    vk::SemaphoreSubmitInfo()
                        .setSemaphore(vkSignalSemaphore.getVkSemaphore())
                        .setStageMask(
                            toVkPipelineStageFlags(signalBinarySemaphoreInfo.signalStage)));
            }
            for (const auto& signalTimelineSemaphoreInfo :
                 submitDescription.signalTimelineSemaphores) {
                if (!signalTimelineSemaphoreInfo.semaphore) {
                    throw std::invalid_argument("Timeline semaphore pointer is null");
                }
                auto& vkSignalSemaphore = dynamic_cast<VulkanTimelineSemaphore&>(
                    *signalTimelineSemaphoreInfo.semaphore);

                vkSignalSemaphoreInfos.push_back(
                    vk::SemaphoreSubmitInfo()
                        .setSemaphore(vkSignalSemaphore.getVkSemaphore())
                        .setStageMask(
                            toVkPipelineStageFlags(signalTimelineSemaphoreInfo.signalStage))
                        .setValue(signalTimelineSemaphoreInfo.value));
            }

            vkSubmitInfos.push_back(vk::SubmitInfo2()
                                        .setWaitSemaphoreInfos(vkWaitSemaphoreInfos)
                                        .setCommandBufferInfos(vkCommandBufferInfos)
                                        .setSignalSemaphoreInfos(vkSignalSemaphoreInfos));
        }

        if (vkFence) {
            queue_.submit2(vkSubmitInfos, vkFence->getVkFence());
        } else {
            queue_.submit2(vkSubmitInfos);
        }
    }

    std::pair<QueuePresentResultCode, std::vector<QueuePresentResultCode>> VulkanQueue::present(
        const QueuePresentDescription& presentDescription) {
        std::vector<vk::Semaphore> vkWaitSemaphores;
        vkWaitSemaphores.reserve(presentDescription.waitSemaphores.size());
        std::vector<vk::SwapchainKHR> vkSwapchains;
        vkSwapchains.reserve(presentDescription.swapchains.size());
        std::vector<uint32_t> vkImageIndices;
        vkImageIndices.reserve(presentDescription.swapchains.size());

        for (const auto& waitSemaphore : presentDescription.waitSemaphores) {
            if (!waitSemaphore) {
                throw std::invalid_argument("Binary semaphore pointer is null");
            }
            auto& vkWaitSemaphore = dynamic_cast<VulkanBinarySemaphore&>(*waitSemaphore);

            vkWaitSemaphores.push_back(vkWaitSemaphore.getVkSemaphore());
        }
        for (const auto& [swapchain, imageIndex] : presentDescription.swapchains) {
            if (!swapchain) {
                throw std::invalid_argument("Swapchain pointer is null");
            }
            auto& vkSwapchain = dynamic_cast<VulkanSwapchain&>(*swapchain);

            vkSwapchains.push_back(vkSwapchain.getVkSwapchain());
            vkImageIndices.push_back(imageIndex);
        }

        std::vector<vk::Result> vkResults;
        vkResults.resize(vkSwapchains.size());
        auto vkOverallResult = queue_.presentKHR(vk::PresentInfoKHR()
                                                     .setWaitSemaphores(vkWaitSemaphores)
                                                     .setSwapchains(vkSwapchains)
                                                     .setImageIndices(vkImageIndices)
                                                     .setResults(vkResults));

        std::vector<QueuePresentResultCode> results;
        results.reserve(vkResults.size());
        for (const auto& vkResult : vkResults) {
            results.push_back(toQueuePresentResultCode(vkResult));
        }
        QueuePresentResultCode overallResult = toQueuePresentResultCode(vkOverallResult);

        return std::make_pair(overallResult, results);
    }
}  // namespace aetherion