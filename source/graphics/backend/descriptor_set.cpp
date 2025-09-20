#include "aetherion/graphics/backend/descriptor_set.hpp"

namespace aetherion {
    IDescriptorSetLayout::~IDescriptorSetLayout() = default;

    IDescriptorSet::~IDescriptorSet() = default;

    IDescriptorPool::~IDescriptorPool() = default;

    IPushConstantRange::~IPushConstantRange() = default;
}  // namespace aetherion
