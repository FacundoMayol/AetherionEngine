#include "vulkan_buffer_view.hpp"

#include "vulkan_buffer.hpp"
#include "vulkan_device.hpp"
#include "vulkan_render_definitions.hpp"

namespace aetherion {
    VulkanBufferView::VulkanBufferView(VulkanDevice& device,
                                       const BufferViewDescription& description)
        : device_(&device) {
        if (!description.buffer) {
            throw std::invalid_argument("Buffer in BufferViewDescription is null.");
        }

        auto& vkBuffer = dynamic_cast<VulkanBuffer&>(*description.buffer);

        bufferView_
            = device_->getVkDevice().createBufferView(vk::BufferViewCreateInfo()
                                                          .setBuffer(vkBuffer.getVkBuffer())
                                                          .setFormat(toVkFormat(description.format))
                                                          .setOffset(description.offset)
                                                          .setRange(description.range));
    }

    VulkanBufferView::VulkanBufferView(VulkanDevice& device, vk::BufferView bufferView)
        : device_(&device), bufferView_(bufferView) {}

    VulkanBufferView::~VulkanBufferView() noexcept { clear(); }

    VulkanBufferView::VulkanBufferView(VulkanBufferView&& other) noexcept
        : IBufferView(std::move(other)), device_(other.device_), bufferView_(other.bufferView_) {
        other.device_ = nullptr;
        other.bufferView_ = nullptr;
    }

    VulkanBufferView& VulkanBufferView::operator=(VulkanBufferView&& other) noexcept {
        if (this != &other) {
            clear();

            IBufferView::operator=(std::move(other));
            device_ = other.device_;
            bufferView_ = other.bufferView_;

            other.release();
        }
        return *this;
    }

    void VulkanBufferView::clear() noexcept {
        if (bufferView_ && device_) {
            device_->getVkDevice().destroyBufferView(bufferView_);
            bufferView_ = nullptr;
        }
        device_ = nullptr;
    }

    void VulkanBufferView::release() noexcept {
        bufferView_ = nullptr;
        device_ = nullptr;
    }
}  // namespace aetherion