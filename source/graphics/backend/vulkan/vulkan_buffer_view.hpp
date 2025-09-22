#pragma once

#include <vulkan/vulkan.hpp>

#include "aetherion/graphics/backend/buffer_view.hpp"

namespace aetherion {
    // Forward declarations
    class VulkanDevice;
    class VulkanBuffer;

    class VulkanBufferView : public IRenderBufferView {
      public:
        VulkanBufferView() = delete;
        VulkanBufferView(VulkanDevice& device, const RenderBufferViewDescription& description);
        VulkanBufferView(vk::Device device, vk::BufferView bufferView);
        ~VulkanBufferView() noexcept override;

        VulkanBufferView(const VulkanBufferView&) = delete;
        VulkanBufferView& operator=(const VulkanBufferView&) = delete;

        VulkanBufferView(VulkanBufferView&&) noexcept;
        VulkanBufferView& operator=(VulkanBufferView&&) noexcept;

        inline vk::BufferView getVkBufferView() const { return bufferView_; }

        void clear() noexcept;
        void release() noexcept;

      private:
        vk::Device device_;

        vk::BufferView bufferView_;
    };
}  // namespace aetherion
