#include "vulkan_descriptor_set.hpp"

#include "vulkan_device.hpp"
#include "vulkan_render_definitions.hpp"

namespace aetherion {
    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
        VulkanDevice& device, const DescriptorSetLayoutDescription& description)
        : device_(device.getVkDevice()) {
        std::vector<vk::DescriptorSetLayoutBinding> vkBindings;
        vkBindings.reserve(description.bindings.size());

        for (const auto& binding : description.bindings) {
            vkBindings.push_back(vk::DescriptorSetLayoutBinding()
                                     .setBinding(binding.binding)
                                     .setDescriptorType(toVkDescriptorType(binding.type))
                                     .setDescriptorCount(binding.count)
                                     .setStageFlags(toVkShaderStageFlags(binding.stages))
                                     .setPImmutableSamplers(nullptr));
        }

        descriptorSetLayout_ = device_.createDescriptorSetLayout(
            vk::DescriptorSetLayoutCreateInfo().setBindings(vkBindings));
    }

    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
        vk::Device device, vk::DescriptorSetLayout descriptorSetLayout)
        : device_(device), descriptorSetLayout_(descriptorSetLayout) {}

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() noexcept { clear(); }

    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&& other) noexcept
        : IDescriptorSetLayout(std::move(other)),
          device_(other.device_),
          descriptorSetLayout_(other.descriptorSetLayout_) {
        other.device_ = nullptr;
        other.descriptorSetLayout_ = nullptr;
    }

    VulkanDescriptorSetLayout& VulkanDescriptorSetLayout::operator=(
        VulkanDescriptorSetLayout&& other) noexcept {
        if (this != &other) {
            clear();

            IDescriptorSetLayout::operator=(std::move(other));
            device_ = other.device_;
            descriptorSetLayout_ = other.descriptorSetLayout_;

            other.release();
        }
        return *this;
    }

    void VulkanDescriptorSetLayout::clear() noexcept {
        if (descriptorSetLayout_ && device_) {
            device_.destroyDescriptorSetLayout(descriptorSetLayout_);
            descriptorSetLayout_ = nullptr;
        }
        device_ = nullptr;
    }

    void VulkanDescriptorSetLayout::release() noexcept {
        descriptorSetLayout_ = nullptr;
        device_ = nullptr;
    }

    std::vector<std::unique_ptr<IDescriptorSet>> VulkanDescriptorSet::allocateDescriptorSets(
        IRenderDevice& device, IDescriptorPool& pool,
        std::span<const DescriptorSetDescription> descriptions) {
        return allocateDescriptorSets(
            dynamic_cast<VulkanDevice&>(device).getVkDevice(),
            dynamic_cast<VulkanDescriptorPool&>(pool).getVkDescriptorPool(), descriptions);
    }

    std::vector<std::unique_ptr<IDescriptorSet>> VulkanDescriptorSet::allocateDescriptorSets(
        VulkanDevice& device, VulkanDescriptorPool& pool,
        std::span<const DescriptorSetDescription> descriptions) {
        return allocateDescriptorSets(device.getVkDevice(), pool.getVkDescriptorPool(),
                                      descriptions);
    }

    std::vector<std::unique_ptr<IDescriptorSet>> VulkanDescriptorSet::allocateDescriptorSets(
        vk::Device device, vk::DescriptorPool pool,
        std::span<const DescriptorSetDescription> descriptions) {
        std::vector<vk::DescriptorSetLayout> vkLayouts;
        vkLayouts.reserve(descriptions.size());

        for (const auto& description : descriptions) {
            if (!description.layout) {
                throw std::invalid_argument("layout in DescriptorSetDescription is null.");
            }
            const auto* vkLayout
                = dynamic_cast<const VulkanDescriptorSetLayout*>(description.layout);
            vkLayouts.push_back(vkLayout->getVkDescriptorSetLayout());
        }

        auto result = device.allocateDescriptorSets(
            vk::DescriptorSetAllocateInfo().setDescriptorPool(pool).setSetLayouts(vkLayouts));

        std::vector<std::unique_ptr<IDescriptorSet>> descriptorSets;
        descriptorSets.reserve(result.size());

        for (const auto& descriptorSet : result) {
            descriptorSets.push_back(
                std::make_unique<VulkanDescriptorSet>(device, pool, descriptorSet));
        }

        return descriptorSets;
    }

    VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice& device, VulkanDescriptorPool& pool,
                                             const DescriptorSetDescription& description)
        : device_(device.getVkDevice()),
          pool_(pool.getVkDescriptorPool()),
          shouldFreeDescriptorSet(pool.supportsFreeDescriptorSet()) {
        if (!description.layout) {
            throw std::invalid_argument("layout in DescriptorSetDescription is null.");
        }
        const auto* vkLayout = dynamic_cast<const VulkanDescriptorSetLayout*>(description.layout);

        auto vkLayoutHandle = vkLayout->getVkDescriptorSetLayout();

        auto result
            = device_.allocateDescriptorSets(vk::DescriptorSetAllocateInfo()
                                                 .setDescriptorPool(pool.getVkDescriptorPool())
                                                 .setDescriptorSetCount(1)
                                                 .setPSetLayouts(&vkLayoutHandle));

        descriptorSet_ = result.front();
    }

    VulkanDescriptorSet::VulkanDescriptorSet(vk::Device device, vk::DescriptorPool pool,
                                             vk::DescriptorSet descriptorSet, bool shouldFree)
        : device_(device),
          pool_(pool),
          descriptorSet_(descriptorSet),
          shouldFreeDescriptorSet(shouldFree) {}

    VulkanDescriptorSet::~VulkanDescriptorSet() noexcept { clear(); }

    VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorSet&& other) noexcept
        : IDescriptorSet(std::move(other)),
          device_(other.device_),
          pool_(other.pool_),
          descriptorSet_(other.descriptorSet_),
          shouldFreeDescriptorSet(other.shouldFreeDescriptorSet) {
        other.device_ = nullptr;
        other.pool_ = nullptr;
        other.descriptorSet_ = nullptr;
        other.shouldFreeDescriptorSet = false;
    }

    VulkanDescriptorSet& VulkanDescriptorSet::operator=(VulkanDescriptorSet&& other) noexcept {
        if (this != &other) {
            clear();

            IDescriptorSet::operator=(std::move(other));
            device_ = other.device_;
            pool_ = other.pool_;
            descriptorSet_ = other.descriptorSet_;
            shouldFreeDescriptorSet = other.shouldFreeDescriptorSet;

            other.release();
        }
        return *this;
    }

    void VulkanDescriptorSet::clear() noexcept {
        if (descriptorSet_ && device_ && pool_ && shouldFreeDescriptorSet) {
            device_.freeDescriptorSets(pool_, 1, &descriptorSet_);
            descriptorSet_ = nullptr;
        }
        device_ = nullptr;
        pool_ = nullptr;
        shouldFreeDescriptorSet = false;
    }

    void VulkanDescriptorSet::release() noexcept {
        descriptorSet_ = nullptr;
        device_ = nullptr;
        pool_ = nullptr;
        shouldFreeDescriptorSet = false;
    }

    void VulkanDescriptorSet::freeDescriptorSets(
        IRenderDevice& device, IDescriptorPool& pool,
        std::span<std::reference_wrapper<IDescriptorSet>> descriptorSets) {
        std::vector<vk::DescriptorSet> vkDescriptorSets;
        vkDescriptorSets.reserve(descriptorSets.size());

        for (auto& descriptorSet : descriptorSets) {
            auto& vkDescriptorSet = dynamic_cast<VulkanDescriptorSet&>(descriptorSet.get());
            vkDescriptorSets.push_back(vkDescriptorSet.getVkDescriptorSet());
            vkDescriptorSet.release();
        }

        freeDescriptorSets(dynamic_cast<VulkanDevice&>(device).getVkDevice(),
                           dynamic_cast<VulkanDescriptorPool&>(pool).getVkDescriptorPool(),
                           vkDescriptorSets);
    }

    void VulkanDescriptorSet::freeDescriptorSets(
        VulkanDevice& device, VulkanDescriptorPool& pool,
        std::span<std::reference_wrapper<VulkanDescriptorSet>> descriptorSets) {
        std::vector<vk::DescriptorSet> vkDescriptorSets;
        vkDescriptorSets.reserve(descriptorSets.size());

        for (auto& descriptorSet : descriptorSets) {
            vkDescriptorSets.push_back(descriptorSet.get().getVkDescriptorSet());
            descriptorSet.get().release();
        }

        freeDescriptorSets(device.getVkDevice(), pool.getVkDescriptorPool(), vkDescriptorSets);
    }

    void VulkanDescriptorSet::freeDescriptorSets(vk::Device device, vk::DescriptorPool pool,
                                                 std::span<vk::DescriptorSet> descriptorSets) {
        device.freeDescriptorSets(pool, descriptorSets);
    }

    VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice& device,
                                               const DescriptorPoolDescription& description)
        : device_(device.getVkDevice()),
          freeDescriptorSetSupport_(
              description.flags.contains(DescriptorPoolBehavior::FreeIndividualSets)) {
        std::vector<vk::DescriptorPoolSize> poolSizes;
        poolSizes.reserve(description.poolSizes.size());
        for (const auto& size : description.poolSizes) {
            poolSizes.push_back(vk::DescriptorPoolSize()
                                    .setType(toVkDescriptorType(size.type))
                                    .setDescriptorCount(size.count));
        }

        vk::DescriptorPoolCreateInfo poolCreateInfo
            = vk::DescriptorPoolCreateInfo()
                  .setMaxSets(description.maxSets)
                  .setPoolSizes(poolSizes)
                  .setFlags(toVkDescriptorPoolCreateFlags(description.flags));

        descriptorPool_ = device_.createDescriptorPool(poolCreateInfo);
    }

    VulkanDescriptorPool::VulkanDescriptorPool(vk::Device device, vk::DescriptorPool descriptorPool,
                                               bool freeDescriptorSetSupport)
        : device_(device),
          descriptorPool_(descriptorPool),
          freeDescriptorSetSupport_(freeDescriptorSetSupport) {}

    VulkanDescriptorPool::~VulkanDescriptorPool() noexcept { clear(); }

    VulkanDescriptorPool::VulkanDescriptorPool(VulkanDescriptorPool&& other) noexcept
        : IDescriptorPool(std::move(other)),
          device_(other.device_),
          descriptorPool_(other.descriptorPool_),
          freeDescriptorSetSupport_(other.freeDescriptorSetSupport_) {
        other.device_ = nullptr;
        other.descriptorPool_ = nullptr;
        other.freeDescriptorSetSupport_ = false;
    }

    VulkanDescriptorPool& VulkanDescriptorPool::operator=(VulkanDescriptorPool&& other) noexcept {
        if (this != &other) {
            clear();

            IDescriptorPool::operator=(std::move(other));
            device_ = other.device_;
            descriptorPool_ = other.descriptorPool_;
            freeDescriptorSetSupport_ = other.freeDescriptorSetSupport_;

            other.release();
        }
        return *this;
    }

    void VulkanDescriptorPool::reset() { device_.resetDescriptorPool(descriptorPool_); }

    void VulkanDescriptorPool::clear() noexcept {
        if (descriptorPool_ && device_) {
            device_.destroyDescriptorPool(descriptorPool_);
            descriptorPool_ = nullptr;
        }
        device_ = nullptr;
        freeDescriptorSetSupport_ = false;
    }

    void VulkanDescriptorPool::release() noexcept {
        descriptorPool_ = nullptr;
        device_ = nullptr;
        freeDescriptorSetSupport_ = false;
    }

    VulkanPushConstantRange::VulkanPushConstantRange(
        VulkanDevice& device, const PushConstantRangeDescription& description)
        : device_(device.getVkDevice()) {
        pushConstantRange_ = vk::PushConstantRange()
                                 .setOffset(description.offset)
                                 .setSize(description.size)
                                 .setStageFlags(toVkShaderStageFlags(description.stages));
    }

    VulkanPushConstantRange::VulkanPushConstantRange(vk::Device device,
                                                     vk::PushConstantRange pushConstantRange)
        : device_(device), pushConstantRange_(pushConstantRange) {}

    VulkanPushConstantRange::~VulkanPushConstantRange() noexcept { clear(); }

    VulkanPushConstantRange::VulkanPushConstantRange(VulkanPushConstantRange&& other) noexcept
        : IPushConstantRange(std::move(other)),
          device_(other.device_),
          pushConstantRange_(other.pushConstantRange_) {
        other.device_ = nullptr;
        pushConstantRange_ = vk::PushConstantRange();
    }

    VulkanPushConstantRange& VulkanPushConstantRange::operator=(
        VulkanPushConstantRange&& other) noexcept {
        if (this != &other) {
            clear();

            IPushConstantRange::operator=(std::move(other));
            device_ = other.device_;
            pushConstantRange_ = other.pushConstantRange_;

            other.release();
        }
        return *this;
    }

    void VulkanPushConstantRange::clear() noexcept {
        // No Vulkan resources to free
        release();
    }

    void VulkanPushConstantRange::release() noexcept {
        device_ = nullptr;
        pushConstantRange_ = vk::PushConstantRange();
    }
}  // namespace aetherion