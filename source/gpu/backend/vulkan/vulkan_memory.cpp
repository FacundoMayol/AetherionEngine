#include "vulkan_memory.hpp"

#include "vulkan_buffer.hpp"
#include "vulkan_device.hpp"
#include "vulkan_image.hpp"
#include "vulkan_render_definitions.hpp"

namespace aetherion {
    VulkanAllocator::VulkanAllocator(VulkanDevice& device,
                                     const GPUAllocatorDescription& description)
        : physicalDevice_(device.getVkPhysicalDevice()), device_(device.getVkDevice()) {
        allocator_ = vma::createAllocator(
            vma::AllocatorCreateInfo()
                .setPhysicalDevice(physicalDevice_)
                .setDevice(device_)
                .setInstance(device.getVkInstance())
                .setFlags(toVmaAllocatorFlags(description.flags)
                          | vma::AllocatorCreateFlagBits::eBufferDeviceAddress)
                //.setPHeapSizeLimit(&description.heapSizeLimit)
                .setVulkanApiVersion(VK_API_VERSION_1_3));
    }

    VulkanAllocator::VulkanAllocator(vk::PhysicalDevice physicalDevice, vk::Device device,
                                     vma::Allocator allocator)
        : physicalDevice_(physicalDevice), device_(device), allocator_(allocator) {}

    VulkanAllocator::~VulkanAllocator() noexcept { clear(); }

    VulkanAllocator::VulkanAllocator(VulkanAllocator&& other) noexcept
        : IGPUAllocator(std::move(other)),
          physicalDevice_(other.physicalDevice_),
          device_(other.device_),
          allocator_(other.allocator_) {
        other.physicalDevice_ = nullptr;
        other.device_ = nullptr;
        other.allocator_ = nullptr;
    }

    VulkanAllocator& VulkanAllocator::operator=(VulkanAllocator&& other) noexcept {
        if (this != &other) {
            clear();

            IGPUAllocator::operator=(std::move(other));
            physicalDevice_ = other.physicalDevice_;
            device_ = other.device_;
            allocator_ = other.allocator_;

            other.release();
        }
        return *this;
    }

    void VulkanAllocator::clear() noexcept {
        if (allocator_) {
            allocator_.destroy();
        }
        release();
    }

    void VulkanAllocator::release() noexcept {
        physicalDevice_ = nullptr;
        device_ = nullptr;
        allocator_ = nullptr;
    }

    std::unique_ptr<IGPUAllocatorPool> VulkanAllocator::createPool(
        const GPUAllocatorPoolDescription& description) {
        return std::make_unique<VulkanAllocatorPool>(*this, description);
    }

    std::unique_ptr<IGPUAllocation> VulkanAllocator::allocate(
        const GPUAllocationMemoryRequirementsDescription& memoryRequirements,
        const GPUAllocationDescription& description) {
        return std::make_unique<VulkanAllocation>(*this, memoryRequirements, description);
    }

    std::unique_ptr<IGPUAllocation> VulkanAllocator::allocateForImage(
        IGPUImage& image, const GPUAllocationDescription& description) {
        return std::make_unique<VulkanAllocation>(*this, image, description);
    }

    std::unique_ptr<IGPUAllocation> VulkanAllocator::allocateForBuffer(
        IGPUBuffer& buffer, const GPUAllocationDescription& description) {
        return std::make_unique<VulkanAllocation>(*this, buffer, description);
    }

    std::unique_ptr<IGPUImage> VulkanAllocator::createImage(
        const GPUImageDescription& description,
        const GPUAllocationDescription& allocationDescription) {
        return std::make_unique<VulkanImage>(*this, description, allocationDescription);
    }

    std::unique_ptr<IGPUBuffer> VulkanAllocator::createBuffer(
        const GPUBufferDescription& description,
        const GPUAllocationDescription& allocationDescription) {
        return std::make_unique<VulkanBuffer>(*this, description, allocationDescription);
    }

    std::unique_ptr<IGPUImage> VulkanAllocator::createAliasedImage(
        IGPUAllocation& allocation, const GPUImageDescription& description, size_t offset) {
        auto& vkAllocation = dynamic_cast<VulkanAllocation&>(allocation);

        return std::make_unique<VulkanImage>(
            device_, allocator_,
            allocator_.createAliasingImage2(
                vkAllocation.getVmaAllocation(), offset,
                vk::ImageCreateInfo()
                    .setImageType(toVkImageType(description.type))
                    .setFormat(toVkFormat(description.format))
                    .setExtent(toVkExtent3D(description.extent))
                    .setMipLevels(description.mipLevels)
                    .setArrayLayers(description.arrayLayers)
                    .setSamples(toVkSampleCount(description.sampleCount))
                    .setInitialLayout(toVkImageLayout(description.initialLayout))
                    .setTiling(toVkImageTiling(description.tiling))
                    .setUsage(toVkImageUsageFlags(description.usages))
                    .setSharingMode(toVkSharingMode(description.sharingMode))
                    .setQueueFamilyIndices(description.queueFamilies)
                    .setFlags((description.cubeCompatible ? vk::ImageCreateFlagBits::eCubeCompatible
                                                          : vk::ImageCreateFlags())
                              | (description.arrayCompatible
                                     ? vk::ImageCreateFlagBits::e2DArrayCompatible
                                     : vk::ImageCreateFlags())
                              | (description.type == GPUImageType::Tex3d
                                     ? vk::ImageCreateFlagBits::e2DArrayCompatible  // Assuming this
                                                                                    // usage.
                                     : vk::ImageCreateFlags()))),
            nullptr);  // No allocation because aliased and as such
                       // the allocation is managed externally.
    }

    std::unique_ptr<IGPUBuffer> VulkanAllocator::createAliasedBuffer(
        IGPUAllocation& allocation, const GPUBufferDescription& description, size_t offset) {
        auto& vkAllocation = dynamic_cast<VulkanAllocation&>(allocation);

        return std::make_unique<VulkanBuffer>(
            device_, allocator_,
            allocator_.createAliasingBuffer2(
                vkAllocation.getVmaAllocation(), offset,
                vk::BufferCreateInfo()
                    .setSize(description.size)
                    .setUsage(toVkBufferUsageFlags(description.usages))
                    .setSharingMode(toVkSharingMode(description.sharingMode))
                    .setQueueFamilyIndices(description.queueFamilies)),
            nullptr);  // No allocation because aliased and as such
                       // the allocation is managed externally.
    }

    constexpr vk::MemoryRequirements toVkMemoryRequirements(
        const GPUAllocationMemoryRequirementsDescription& memoryRequirements) {
        auto vkMemoryRequirements = vk::MemoryRequirements();
        vkMemoryRequirements.size = memoryRequirements.size;
        vkMemoryRequirements.alignment = memoryRequirements.alignment;
        vkMemoryRequirements.memoryTypeBits = memoryRequirements.memoryTypeBits;
        return vkMemoryRequirements;
    }

    VulkanAllocation::VulkanAllocation(
        VulkanAllocator& allocator,
        const GPUAllocationMemoryRequirementsDescription& memoryRequirements,
        const GPUAllocationDescription& description)
        : allocator_(allocator.getVmaAllocator()) {
        auto* vkAllocatorPool = dynamic_cast<VulkanAllocatorPool*>(description.pool);

        allocation_ = allocator_.allocateMemory(
            toVkMemoryRequirements(memoryRequirements),
            vma::AllocationCreateInfo()
                //.setMemoryTypeBits(description.memoryTypeBits)
                .setPool(vkAllocatorPool ? vkAllocatorPool->getVmaPool() : nullptr)
                .setPriority(description.priority)
                .setUsage(toVmaMemoryUsage(description.memoryUsage))
                .setFlags(toVmaAllocationCreateFlags(description.properties)));
    }

    VulkanAllocation::VulkanAllocation(VulkanAllocator& allocator, const IGPUImage& image,
                                       const GPUAllocationDescription& description)
        : allocator_(allocator.getVmaAllocator()) {
        auto* vkAllocatorPool = dynamic_cast<VulkanAllocatorPool*>(description.pool);

        allocation_ = allocator_.allocateMemoryForImage(
            dynamic_cast<const VulkanImage&>(image).getVkImage(),
            vma::AllocationCreateInfo()
                //.setMemoryTypeBits(description.memoryTypeBits)
                .setPool(vkAllocatorPool ? vkAllocatorPool->getVmaPool() : nullptr)
                .setPriority(description.priority)
                .setUsage(toVmaMemoryUsage(description.memoryUsage))
                .setFlags(toVmaAllocationCreateFlags(description.properties)));
    }

    VulkanAllocation::VulkanAllocation(VulkanAllocator& allocator, const IGPUBuffer& buffer,
                                       const GPUAllocationDescription& description)
        : allocator_(allocator.getVmaAllocator()) {
        auto* vkAllocatorPool = dynamic_cast<VulkanAllocatorPool*>(description.pool);

        allocation_ = allocator_.allocateMemoryForBuffer(
            dynamic_cast<const VulkanBuffer&>(buffer).getVkBuffer(),
            vma::AllocationCreateInfo()
                //.setMemoryTypeBits(description.memoryTypeBits)
                .setPool(vkAllocatorPool ? vkAllocatorPool->getVmaPool() : nullptr)
                .setPriority(description.priority)
                .setUsage(toVmaMemoryUsage(description.memoryUsage))
                .setFlags(toVmaAllocationCreateFlags(description.properties)));
    }

    VulkanAllocation::VulkanAllocation(vma::Allocator allocator, vma::Allocation allocation)
        : allocator_(allocator), allocation_(allocation) {}

    VulkanAllocation::~VulkanAllocation() noexcept { clear(); }

    VulkanAllocation::VulkanAllocation(VulkanAllocation&& other) noexcept
        : IGPUAllocation(std::move(other)),
          allocator_(other.allocator_),
          allocation_(other.allocation_) {
        other.allocator_ = nullptr;
        other.allocation_ = nullptr;
    }

    VulkanAllocation& VulkanAllocation::operator=(VulkanAllocation&& other) noexcept {
        if (this != &other) {
            clear();

            IGPUAllocation::operator=(std::move(other));
            allocator_ = other.allocator_;
            allocation_ = other.allocation_;

            other.release();
        }
        return *this;
    }

    void VulkanAllocation::clear() noexcept {
        if (allocation_ && allocator_) {
            allocator_.freeMemory(allocation_);
        }
        release();
    }

    void VulkanAllocation::release() noexcept {
        allocator_ = nullptr;
        allocation_ = nullptr;
    }

    VulkanAllocatorPool::VulkanAllocatorPool(VulkanAllocator& allocator,
                                             const GPUAllocatorPoolDescription& description)
        : allocator_(allocator.getVmaAllocator()) {
        pool_ = allocator_.createPool(vma::PoolCreateInfo()
                                          //.setMemoryTypeIndex(description.memoryTypeIndex)
                                          .setBlockSize(description.blockSize)
                                          .setMinBlockCount(description.minBlockCount)
                                          .setMaxBlockCount(description.maxBlockCount)
                                          .setFlags(toVmaAllocatorPoolFlags(description.flags)));
    }

    VulkanAllocatorPool::VulkanAllocatorPool(vma::Allocator allocator, vma::Pool pool)
        : allocator_(allocator), pool_(pool) {}

    VulkanAllocatorPool::~VulkanAllocatorPool() noexcept { clear(); }

    VulkanAllocatorPool::VulkanAllocatorPool(VulkanAllocatorPool&& other) noexcept
        : IGPUAllocatorPool(std::move(other)), allocator_(other.allocator_), pool_(other.pool_) {
        other.allocator_ = nullptr;
        other.pool_ = nullptr;
    }

    VulkanAllocatorPool& VulkanAllocatorPool::operator=(VulkanAllocatorPool&& other) noexcept {
        if (this != &other) {
            clear();

            IGPUAllocatorPool::operator=(std::move(other));
            allocator_ = other.allocator_;
            pool_ = other.pool_;

            other.release();
        }
        return *this;
    }

    void VulkanAllocatorPool::clear() noexcept {
        if (pool_ && allocator_) {
            allocator_.destroyPool(pool_);
        }
        release();
    }

    void VulkanAllocatorPool::release() noexcept {
        pool_ = nullptr;
        allocator_ = nullptr;
    }
}  // namespace aetherion