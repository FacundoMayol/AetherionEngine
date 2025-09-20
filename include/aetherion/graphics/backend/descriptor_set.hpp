#pragma once

#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"

namespace aetherion {
    // Forward declarations
    class IDescriptorPool;
    class IDescriptorSetLayout;
    class IImage;

    struct DescriptorPoolSize {
        DescriptorType type;
        uint32_t count;
    };

    struct DescriptorPoolDescription {
        uint32_t maxSets;
        std::vector<DescriptorPoolSize> poolSizes;
        DescriptorPoolBehaviorFlags flags;
    };

    struct DescriptorSetLayoutBindingDescription {
        uint32_t binding;
        DescriptorType type;
        uint32_t count;
        ShaderStageFlags stages;
    };

    struct DescriptorSetLayoutDescription {
        std::vector<DescriptorSetLayoutBindingDescription> bindings;
    };

    struct PushConstantRangeDescription {
        uint32_t offset;
        uint32_t size;
        ShaderStageFlags stages;
    };

    struct DescriptorSetDescription {
        IDescriptorSetLayout* layout;
    };

    class IDescriptorSetLayout {
      public:
        virtual ~IDescriptorSetLayout() = 0;

        IDescriptorSetLayout(const IDescriptorSetLayout&) = delete;
        IDescriptorSetLayout& operator=(const IDescriptorSetLayout&) = delete;

      protected:
        IDescriptorSetLayout() = default;
        IDescriptorSetLayout(IDescriptorSetLayout&&) noexcept = default;
        IDescriptorSetLayout& operator=(IDescriptorSetLayout&&) noexcept = default;
    };

    class IDescriptorSet : public IResource {
      public:
        virtual ~IDescriptorSet() = 0;

        IDescriptorSet(const IDescriptorSet&) = delete;
        IDescriptorSet& operator=(const IDescriptorSet&) = delete;

      protected:
        IDescriptorSet() = default;
        IDescriptorSet(IDescriptorSet&&) noexcept = default;
        IDescriptorSet& operator=(IDescriptorSet&&) noexcept = default;
    };

    class IDescriptorPool : public IResource {
      public:
        virtual ~IDescriptorPool() = 0;

        IDescriptorPool(const IDescriptorPool&) = delete;
        IDescriptorPool& operator=(const IDescriptorPool&) = delete;

        virtual void reset() = 0;

      protected:
        IDescriptorPool() = default;
        IDescriptorPool(IDescriptorPool&&) noexcept = default;
        IDescriptorPool& operator=(IDescriptorPool&&) noexcept = default;
    };

    class IPushConstantRange {
      public:
        virtual ~IPushConstantRange() = 0;

        IPushConstantRange(const IPushConstantRange&) = delete;
        IPushConstantRange& operator=(const IPushConstantRange&) = delete;

      protected:
        IPushConstantRange() = default;
        IPushConstantRange(IPushConstantRange&&) noexcept = default;
        IPushConstantRange& operator=(IPushConstantRange&&) noexcept = default;
    };
}  // namespace aetherion
