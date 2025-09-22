#include "vulkan_device.hpp"

#include <fmt/core.h>

#include <stdexcept>

#include "aetherion/platform/window.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_buffer_view.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_descriptor_set.hpp"
#include "vulkan_driver.hpp"
#include "vulkan_image.hpp"
#include "vulkan_image_view.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_queue.hpp"
#include "vulkan_render_definitions.hpp"
#include "vulkan_sampler.hpp"
#include "vulkan_shader.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_sync.hpp"

namespace aetherion {
    void populateQueueFamilyProperties(
        vk::PhysicalDevice physicalDevice,
        std::unordered_map<uint32_t, QueueFamilyProperties>& queueFamilyProperties) {
        const auto& vkQueueFamilyProperties = physicalDevice.getQueueFamilyProperties();

        for (size_t i = 0; i < vkQueueFamilyProperties.size(); ++i) {
            const auto& vulkanProperties = vkQueueFamilyProperties[i];

            queueFamilyProperties[static_cast<uint32_t>(i)]
                = {.queueFlags = toQueueTypeFlags(vulkanProperties.queueFlags),
                   .queueCount = vulkanProperties.queueCount};
        }
    }

    VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanDriver& driver,
                                               const PhysicalDeviceDescription& description)
        : instance_(driver.getVkInstance()) {
        // Temporal surface creation

        if (!description.primaryWindow) {
            throw(
                std::invalid_argument("A primary window is required to create a physical device."));
        }

        const auto& primaryWindow = *description.primaryWindow;
        vk::SurfaceKHR surface = primaryWindow.createVulkanSurface(driver.getVkInstance());

        // Physical device selection

        // TODO: Allow more customization of the selection process.
        const auto& vkPhysicalDeviceSelectorResult
            = vkb::PhysicalDeviceSelector(driver.getVkBuilderInstance())
                  .set_surface(surface)
                  .set_minimum_version(1, 3)
                  .add_required_extension(vk::KHRSwapchainExtensionName)
                  .set_required_features(vk::PhysicalDeviceFeatures()
                                             .setSamplerAnisotropy(vk::True)
                                             .setFillModeNonSolid(vk::True))
                  .set_required_features_12(
                      vk::PhysicalDeviceVulkan12Features().setBufferDeviceAddress(vk::True))
                  .set_required_features_13(vk::PhysicalDeviceVulkan13Features()
                                                .setDynamicRendering(vk::True)
                                                .setSynchronization2(vk::True))
                  .select();

        if (!vkPhysicalDeviceSelectorResult) {
            throw(
                std::runtime_error(fmt::format("Failed to pick Vulkan physical device. Error: {}",
                                               vkPhysicalDeviceSelectorResult.error().message())));
        }

        builderPhysicalDevice_ = vkPhysicalDeviceSelectorResult.value();
        physicalDevice_ = vk::PhysicalDevice(builderPhysicalDevice_.physical_device);

        // Queue family properties

        populateQueueFamilyProperties(physicalDevice_, queueFamilyProperties_);

        // Temporary surface destruction

        vkb::destroy_surface(driver.getVkBuilderInstance(), surface);
    }

    VulkanPhysicalDevice::VulkanPhysicalDevice(vk::Instance instance,
                                               vkb::PhysicalDevice builderPhysicalDevice,
                                               vk::PhysicalDevice physicalDevice)
        : instance_(instance),
          builderPhysicalDevice_(builderPhysicalDevice),
          physicalDevice_(physicalDevice) {
        // Queue family properties

        populateQueueFamilyProperties(physicalDevice_, queueFamilyProperties_);
    }

    VulkanPhysicalDevice::~VulkanPhysicalDevice() noexcept { clear(); }

    VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanPhysicalDevice&& other) noexcept
        : IPhysicalDevice(std::move(other)),
          builderPhysicalDevice_(std::move(other.builderPhysicalDevice_)),
          physicalDevice_(other.physicalDevice_),
          queueFamilyProperties_(std::move(other.queueFamilyProperties_)),
          instance_(other.instance_) {
        other.physicalDevice_ = nullptr;
        other.instance_ = nullptr;
    }

    VulkanPhysicalDevice& VulkanPhysicalDevice::operator=(VulkanPhysicalDevice&& other) noexcept {
        if (this != &other) {
            clear();

            IPhysicalDevice::operator=(std::move(other));
            builderPhysicalDevice_ = std::move(other.builderPhysicalDevice_);
            physicalDevice_ = other.physicalDevice_;
            queueFamilyProperties_ = std::move(other.queueFamilyProperties_);
            instance_ = other.instance_;

            other.release();
        }
        return *this;
    }

    void VulkanPhysicalDevice::clear() noexcept {
        // NOTE: Physical devices are not owned, so we don't destroy them.
        release();
    }

    void VulkanPhysicalDevice::release() noexcept {
        builderPhysicalDevice_ = {};
        physicalDevice_ = nullptr;
        queueFamilyProperties_.clear();
        instance_ = nullptr;
    }

    const QueueFamilyProperties& VulkanPhysicalDevice::getQueueFamilyProperties(
        uint32_t familyIndex) const {
        auto it = queueFamilyProperties_.find(familyIndex);
        if (it == queueFamilyProperties_.end()) {
            throw(std::out_of_range(
                fmt::format("Queue family index {} is out of range.", familyIndex)));
        }
        return it->second;
    }

    VulkanDevice::VulkanDevice(VulkanDriver& driver, const DeviceDescription& description)
        : instance_(driver.getVkInstance()) {
        if (!description.physicalDevice) {
            throw(
                std::invalid_argument("A physical device is required to create a render device."));
        }
        const auto& physicalDevice
            = dynamic_cast<const VulkanPhysicalDevice&>(*description.physicalDevice);

        // Logical device creation

        std::vector<vkb::CustomQueueDescription> customQueueDescriptions;
        customQueueDescriptions.reserve(description.queueFamilyDescriptions.size());
        for (const auto& queueFamily : description.queueFamilyDescriptions) {
            if (queueFamily.queuePriorities.empty()) {
                throw std::runtime_error(fmt::format(
                    "Queue family index {} in RenderDeviceDescription has zero priorities.",
                    queueFamily.queueFamilyIndex));
            }
            if (queueFamily.queuePriorities.size()
                > physicalDevice.getQueueFamilyProperties(queueFamily.queueFamilyIndex)
                      .queueCount) {
                throw std::runtime_error(fmt::format(
                    "Queue family index {} in RenderDeviceDescription has more priorities "
                    "than available queues.",
                    queueFamily.queueFamilyIndex));
            }

            customQueueDescriptions.push_back(vkb::CustomQueueDescription(
                queueFamily.queueFamilyIndex, queueFamily.queuePriorities));
        }

        const auto& vulkanDeviceBuilderResult
            = vkb::DeviceBuilder(physicalDevice.getVkBuilderPhysicalDevice())
                  .custom_queue_setup(customQueueDescriptions)
                  .build();

        if (!vulkanDeviceBuilderResult) {
            throw(
                std::runtime_error(fmt::format("Failed to create Vulkan logical device. Error: {}",
                                               vulkanDeviceBuilderResult.error().message())));
        }

        builderDevice_ = vulkanDeviceBuilderResult.value();
        device_ = vk::Device(builderDevice_.device);
        physicalDevice_ = physicalDevice.getVkPhysicalDevice();

        // Vulkan Memory Allocator

        allocator_ = vma::createAllocator(
            vma::AllocatorCreateInfo()
                .setPhysicalDevice(physicalDevice.getVkPhysicalDevice())
                .setDevice(device_)
                .setInstance(instance_)
                .setFlags(vma::AllocatorCreateFlagBits::eBufferDeviceAddress));
    }

    VulkanDevice::VulkanDevice(vk::Instance instance, vk::PhysicalDevice physicalDevice,
                               vkb::Device builderDevice, vk::Device device,
                               vma::Allocator allocator)
        : instance_(instance),
          physicalDevice_(physicalDevice),
          builderDevice_(builderDevice),
          device_(device),
          allocator_(allocator) {}

    VulkanDevice::~VulkanDevice() noexcept { clear(); }

    VulkanDevice::VulkanDevice(VulkanDevice&& other) noexcept
        : IRenderDevice(std::move(other)),
          allocator_(other.allocator_),
          builderDevice_(std::move(other.builderDevice_)),
          device_(other.device_),
          instance_(other.instance_),
          physicalDevice_(other.physicalDevice_) {
        other.allocator_ = nullptr;
        other.device_ = nullptr;
        other.instance_ = nullptr;
        other.physicalDevice_ = nullptr;
    }

    VulkanDevice& VulkanDevice::operator=(VulkanDevice&& other) noexcept {
        if (this != &other) {
            clear();

            IRenderDevice::operator=(std::move(other));
            allocator_ = other.allocator_;
            builderDevice_ = std::move(other.builderDevice_);
            device_ = other.device_;
            instance_ = other.instance_;
            physicalDevice_ = other.physicalDevice_;

            other.allocator_ = nullptr;
            other.device_ = nullptr;
            other.instance_ = nullptr;
            other.physicalDevice_ = nullptr;
        }
        return *this;
    }

    void VulkanDevice::clear() noexcept {
        if (device_) {
            if (allocator_) {
                allocator_.destroy();
                allocator_ = nullptr;
            }
            device_.destroy();

            device_ = nullptr;
            builderDevice_ = {};
            instance_ = nullptr;
            physicalDevice_ = nullptr;
        }
    }

    void VulkanDevice::release() noexcept {
        allocator_ = nullptr;
        device_ = nullptr;
        builderDevice_ = {};
        instance_ = nullptr;
        physicalDevice_ = nullptr;
    }

    void VulkanDevice::waitIdle() { device_.waitIdle(); }

    std::unique_ptr<IRenderBuffer> VulkanDevice::createBuffer(
        const BufferDescription& description) {
        return std::make_unique<VulkanBuffer>(*this, description);
    }

    std::unique_ptr<IRenderBufferView> VulkanDevice::createBufferView(
        const RenderBufferViewDescription& description) {
        return std::make_unique<VulkanBufferView>(*this, description);
    }

    std::unique_ptr<IRenderImageView> VulkanDevice::createImageView(
        const RenderImageViewDescription& description) {
        return std::make_unique<VulkanImageView>(*this, description);
    }

    std::unique_ptr<ICommandPool> VulkanDevice::createCommandPool(
        const CommandPoolDescription& description) {
        return std::make_unique<VulkanCommandPool>(*this, description);
    }

    std::unique_ptr<ICommandBuffer> VulkanDevice::allocateCommandBuffer(
        ICommandPool& pool, const CommandBufferDescription& description) {
        return std::make_unique<VulkanCommandBuffer>(*this, dynamic_cast<VulkanCommandPool&>(pool),
                                                     description);
    }

    std::vector<std::unique_ptr<ICommandBuffer>> VulkanDevice::allocateCommandBuffers(
        ICommandPool& pool, uint32_t count, const CommandBufferDescription& description) {
        return VulkanCommandBuffer::allocateCommandBuffers(
            *this, dynamic_cast<VulkanCommandPool&>(pool), count, description);
    }

    void VulkanDevice::freeCommandBuffers(
        ICommandPool& pool, std::span<std::reference_wrapper<ICommandBuffer>> commandBuffers) {
        VulkanCommandBuffer::freeCommandBuffers(*this, dynamic_cast<VulkanCommandPool&>(pool),
                                                commandBuffers);
    }

    std::unique_ptr<IRenderImage> VulkanDevice::createImage(const ImageDescription& description) {
        return std::make_unique<VulkanImage>(*this, description);
    }

    std::unique_ptr<ISwapchain> VulkanDevice::createSwapchain(
        const SwapchainDescription& description) {
        return std::make_unique<VulkanSwapchain>(*this, description);
    }

    std::unique_ptr<IGPUFence> VulkanDevice::createGPUFence(
        const GPUFenceDescription& description) {
        return std::make_unique<VulkanFence>(*this, description);
    }

    std::unique_ptr<IGPUBinarySemaphore> VulkanDevice::createGPUBinarySemaphore(
        const GPUBinarySemaphoreDescription& description) {
        return std::make_unique<VulkanBinarySemaphore>(*this, description);
    }

    std::unique_ptr<IGPUTimelineSemaphore> VulkanDevice::createGPUTimelineSemaphore(
        const GPUTimelineSemaphoreDescription& description) {
        return std::make_unique<VulkanTimelineSemaphore>(*this, description);
    }

    std::unique_ptr<ISampler> VulkanDevice::createSampler(const SamplerDescription& description) {
        return std::make_unique<VulkanSampler>(*this, description);
    }

    std::unique_ptr<IShader> VulkanDevice::createShader(const ShaderDescription& description) {
        return std::make_unique<VulkanShader>(*this, description);
    }

    std::unique_ptr<IPipelineLayout> VulkanDevice::createPipelineLayout(
        const PipelineLayoutDescription& description) {
        return std::make_unique<VulkanPipelineLayout>(*this, description);
    }

    std::unique_ptr<IPipeline> VulkanDevice::createComputePipeline(
        const ComputePipelineDescription& description) {
        return std::make_unique<VulkanPipeline>(*this, description);
    }

    std::unique_ptr<IPipeline> VulkanDevice::createGraphicsPipeline(
        const GraphicsPipelineDescription& description) {
        return std::make_unique<VulkanPipeline>(*this, description);
    }

    std::unique_ptr<IDescriptorSetLayout> VulkanDevice::createDescriptorSetLayout(
        const DescriptorSetLayoutDescription& description) {
        return std::make_unique<VulkanDescriptorSetLayout>(*this, description);
    }

    std::unique_ptr<IDescriptorPool> VulkanDevice::createDescriptorPool(
        const DescriptorPoolDescription& description) {
        return std::make_unique<VulkanDescriptorPool>(*this, description);
    }

    std::unique_ptr<IPushConstantRange> VulkanDevice::createPushConstantRange(
        const PushConstantRangeDescription& description) {
        return std::make_unique<VulkanPushConstantRange>(*this, description);
    }

    std::unique_ptr<IQueue> VulkanDevice::getQueue(const QueueDescription& description) {
        return std::make_unique<VulkanQueue>(*this, description);
    }

    std::unique_ptr<IDescriptorSet> VulkanDevice::allocateDescriptorSet(
        IDescriptorPool& pool, const DescriptorSetDescription& description) {
        return std::make_unique<VulkanDescriptorSet>(
            *this, dynamic_cast<VulkanDescriptorPool&>(pool), description);
    }

    std::vector<std::unique_ptr<IDescriptorSet>> VulkanDevice::allocateDescriptorSets(
        IDescriptorPool& pool, std::span<const DescriptorSetDescription> descriptions) {
        return VulkanDescriptorSet::allocateDescriptorSets(
            *this, dynamic_cast<VulkanDescriptorPool&>(pool), descriptions);
    }

    void VulkanDevice::freeDescriptorSets(
        IDescriptorPool& pool, std::span<std::reference_wrapper<IDescriptorSet>> descriptorSets) {
        VulkanDescriptorSet::freeDescriptorSets(*this, dynamic_cast<VulkanDescriptorPool&>(pool),
                                                descriptorSets);
    }

    vk::WriteDescriptorSet toVkWriteDescriptorSet(const DescriptorWriteDescription& write) {
        if (!write.dstSet) {
            throw std::invalid_argument("dstSet in DescriptorWriteDescription is null.");
        }

        auto vkWrite
            = vk::WriteDescriptorSet()
                  .setDstSet(
                      dynamic_cast<const VulkanDescriptorSet&>(*write.dstSet).getVkDescriptorSet())
                  .setDstBinding(write.dstBinding)
                  .setDstArrayElement(write.dstArrayElement)
                  .setDescriptorType(toVkDescriptorType(write.descriptorType));

        const size_t nonEmptyCount = (!write.buffers.empty() ? 1 : 0)
                                     + (!write.images.empty() ? 1 : 0)
                                     + (!write.texelBuffers.empty() ? 1 : 0);

        if (nonEmptyCount > 1) {
            throw std::invalid_argument(
                "Only one of buffers, images, or texelBuffers fields can be non-empty in "
                "DescriptorWriteDescription.");
        }

        if (!write.buffers.empty()) {
            std::vector<vk::DescriptorBufferInfo> vkBufferInfos;
            vkBufferInfos.reserve(write.buffers.size());
            for (const auto& bufferView : write.buffers) {
                if (!bufferView.buffer) {
                    throw std::invalid_argument(
                        "buffer in DescriptorWriteDescriptorBufferDescription is null.");
                }

                vkBufferInfos.push_back(
                    vk::DescriptorBufferInfo()
                        .setBuffer(
                            dynamic_cast<const VulkanBuffer&>(*bufferView.buffer).getVkBuffer())
                        .setOffset(bufferView.offset)
                        .setRange(bufferView.range));
            }
            return vkWrite.setDescriptorCount(static_cast<uint32_t>(write.buffers.size()))
                .setPBufferInfo(vkBufferInfos.data());
        } else if (!write.images.empty()) {
            std::vector<vk::DescriptorImageInfo> vkImageInfos;
            vkImageInfos.reserve(write.images.size());
            for (const auto& imageView : write.images) {
                if (!imageView.sampler) {
                    throw std::invalid_argument(
                        "sampler in DescriptorWriteDescriptorImageDescription is null.");
                }
                if (!imageView.imageView) {
                    throw std::invalid_argument(
                        "imageView in DescriptorWriteDescriptorImageDescription is null.");
                }

                vkImageInfos.push_back(
                    vk::DescriptorImageInfo()
                        .setSampler(
                            dynamic_cast<const VulkanSampler&>(*imageView.sampler).getVkSampler())
                        .setImageView(dynamic_cast<const VulkanImageView&>(*imageView.imageView)
                                          .getVkImageView())
                        .setImageLayout(toVkImageLayout(imageView.imageLayout)));
            }
            return vkWrite.setDescriptorCount(static_cast<uint32_t>(write.images.size()))
                .setPImageInfo(vkImageInfos.data());
        } else if (!write.texelBuffers.empty()) {
            std::vector<vk::BufferView> vkBufferViews;
            vkBufferViews.reserve(write.texelBuffers.size());
            for (const auto& bufferView : write.texelBuffers) {
                if (!bufferView.bufferView) {
                    throw std::invalid_argument(
                        "bufferView in DescriptorWriteDescriptorTexelBufferViewDescription is "
                        "null.");
                }

                vkBufferViews.push_back(
                    dynamic_cast<const VulkanBufferView&>(*bufferView.bufferView)
                        .getVkBufferView());
            }
            return vkWrite.setDescriptorCount(static_cast<uint32_t>(write.texelBuffers.size()))
                .setPTexelBufferView(vkBufferViews.data());
        } else {
            throw std::invalid_argument(
                "At least one of buffers, images, or texelBuffers fields must be non-empty in "
                "DescriptorWriteDescription.");
        }

        return vkWrite;
    }

    vk::CopyDescriptorSet toVkCopyDescriptorSet(const DescriptorCopyDescription& copy) {
        if (!copy.srcSet) {
            throw std::invalid_argument("srcSet in DescriptorCopyDescription is null.");
        }
        if (!copy.dstSet) {
            throw std::invalid_argument("dstSet in DescriptorCopyDescription is null.");
        }

        return vk::CopyDescriptorSet()
            .setSrcSet(dynamic_cast<const VulkanDescriptorSet&>(*copy.srcSet).getVkDescriptorSet())
            .setSrcBinding(copy.srcBinding)
            .setSrcArrayElement(copy.srcArrayElement)
            .setDstSet(dynamic_cast<const VulkanDescriptorSet&>(*copy.dstSet).getVkDescriptorSet())
            .setDstBinding(copy.dstBinding)
            .setDstArrayElement(copy.dstArrayElement)
            .setDescriptorCount(copy.descriptorCount);
    }

    void VulkanDevice::updateDescriptorSets(
        std::span<const DescriptorWriteDescription> descriptorWrites,
        std::span<const DescriptorCopyDescription> descriptorCopies) {
        std::vector<vk::WriteDescriptorSet> vkDescriptorWrites;
        vkDescriptorWrites.reserve(descriptorWrites.size());

        for (const auto& write : descriptorWrites) {
            vkDescriptorWrites.push_back(toVkWriteDescriptorSet(write));
        }

        std::vector<vk::CopyDescriptorSet> vkDescriptorCopies;
        vkDescriptorCopies.reserve(descriptorCopies.size());

        for (const auto& copy : descriptorCopies) {
            vkDescriptorCopies.push_back(toVkCopyDescriptorSet(copy));
        }

        device_.updateDescriptorSets(vkDescriptorWrites, vkDescriptorCopies);
    }
}  // namespace aetherion