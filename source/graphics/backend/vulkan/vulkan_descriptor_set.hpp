#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/descriptor_set.hpp"

namespace aetherion {
    // Forward declarations
    class IRenderDevice;
    class VulkanDevice;
    class VulkanDescriptorPool;

    class VulkanDescriptorSetLayout : public IDescriptorSetLayout {
      public:
        VulkanDescriptorSetLayout() = delete;
        VulkanDescriptorSetLayout(VulkanDevice& device,
                                  const DescriptorSetLayoutDescription& description);
        VulkanDescriptorSetLayout(vk::Device device, vk::DescriptorSetLayout descriptorSetLayout);
        virtual ~VulkanDescriptorSetLayout() noexcept override;

        VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
        VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

        VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&&) noexcept;
        VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&&) noexcept;

        inline vk::DescriptorSetLayout getVkDescriptorSetLayout() const {
            return descriptorSetLayout_;
        }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;

        vk::DescriptorSetLayout descriptorSetLayout_;
    };

    class VulkanDescriptorSet : public IDescriptorSet {
      public:
        static std::vector<std::unique_ptr<IDescriptorSet>> allocateDescriptorSets(
            IRenderDevice& device, IDescriptorPool& pool,
            std::span<const DescriptorSetDescription> descriptions);
        static std::vector<std::unique_ptr<IDescriptorSet>> allocateDescriptorSets(
            VulkanDevice& device, VulkanDescriptorPool& pool,
            std::span<const DescriptorSetDescription> descriptions);
        static std::vector<std::unique_ptr<IDescriptorSet>> allocateDescriptorSets(
            vk::Device device, vk::DescriptorPool pool,
            std::span<const DescriptorSetDescription> descriptions);

        VulkanDescriptorSet() = delete;
        VulkanDescriptorSet(VulkanDevice& device, VulkanDescriptorPool& pool,
                            const DescriptorSetDescription& description);
        VulkanDescriptorSet(vk::Device device, vk::DescriptorPool pool,
                            vk::DescriptorSet descriptorSet, bool shouldFree = false);
        virtual ~VulkanDescriptorSet() noexcept override;

        VulkanDescriptorSet(const VulkanDescriptorSet&) = delete;
        VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) = delete;

        VulkanDescriptorSet(VulkanDescriptorSet&&) noexcept;
        VulkanDescriptorSet& operator=(VulkanDescriptorSet&&) noexcept;

        static void freeDescriptorSets(
            IRenderDevice& device, IDescriptorPool& pool,
            std::span<std::reference_wrapper<IDescriptorSet>> descriptorSets);
        static void freeDescriptorSets(
            VulkanDevice& device, VulkanDescriptorPool& pool,
            std::span<std::reference_wrapper<VulkanDescriptorSet>> descriptorSets);
        static void freeDescriptorSets(vk::Device device, vk::DescriptorPool pool,
                                       std::span<vk::DescriptorSet> descriptorSets);

        inline vk::DescriptorSet getVkDescriptorSet() const { return descriptorSet_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;
        vk::DescriptorPool pool_;

        vk::DescriptorSet descriptorSet_;

        bool shouldFreeDescriptorSet;
    };

    class VulkanDescriptorPool : public IDescriptorPool {
      public:
        VulkanDescriptorPool() = delete;
        VulkanDescriptorPool(VulkanDevice& device, const DescriptorPoolDescription& description);
        VulkanDescriptorPool(vk::Device device, vk::DescriptorPool descriptorPool,
                             bool freeDescriptorSetSupport);
        virtual ~VulkanDescriptorPool() noexcept override;

        VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
        VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

        VulkanDescriptorPool(VulkanDescriptorPool&&) noexcept;
        VulkanDescriptorPool& operator=(VulkanDescriptorPool&&) noexcept;

        virtual void reset() override;

        inline vk::DescriptorPool getVkDescriptorPool() const { return descriptorPool_; }

        inline bool supportsFreeDescriptorSet() const { return freeDescriptorSetSupport_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;

        vk::DescriptorPool descriptorPool_;

        bool freeDescriptorSetSupport_ = false;
    };

    class VulkanPushConstantRange : public IPushConstantRange {
      public:
        VulkanPushConstantRange() = delete;
        VulkanPushConstantRange(VulkanDevice& device,
                                const PushConstantRangeDescription& description);
        VulkanPushConstantRange(vk::Device device, vk::PushConstantRange pushConstantRange);
        virtual ~VulkanPushConstantRange() noexcept override;

        VulkanPushConstantRange(const VulkanPushConstantRange&) = delete;
        VulkanPushConstantRange& operator=(const VulkanPushConstantRange&) = delete;

        VulkanPushConstantRange(VulkanPushConstantRange&&) noexcept;
        VulkanPushConstantRange& operator=(VulkanPushConstantRange&&) noexcept;

        inline vk::PushConstantRange getVkPushConstantRange() const { return pushConstantRange_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;

        vk::PushConstantRange pushConstantRange_;
    };
}  // namespace aetherion