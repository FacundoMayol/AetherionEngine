#pragma once

#include <cstdint>

#include "aetherion/gpu/backend/resource.hpp"

namespace aetherion {
    struct GPUBinarySemaphoreDescription {
        // PLACEHOLDER
    };

    struct GPUTimelineSemaphoreDescription {
        uint64_t initialValue = 0;
    };

    struct GPUFenceDescription {
        bool signaled = false;
    };

    class IGPUFence : public IGPUResource {
      public:
        ~IGPUFence() override = 0;

        IGPUFence(const IGPUFence&) = delete;
        IGPUFence& operator=(const IGPUFence&) = delete;

        virtual void wait(uint64_t timeout) = 0;
        virtual void reset() = 0;
        virtual bool isSignaled() const = 0;

      protected:
        IGPUFence() = default;
        IGPUFence(IGPUFence&&) noexcept = default;
        IGPUFence& operator=(IGPUFence&&) noexcept = default;
    };

    class IGPUBinarySemaphore : public IGPUResource {
      public:
        ~IGPUBinarySemaphore() override = 0;

        IGPUBinarySemaphore(const IGPUBinarySemaphore&) = delete;
        IGPUBinarySemaphore& operator=(const IGPUBinarySemaphore&) = delete;

      protected:
        IGPUBinarySemaphore() = default;
        IGPUBinarySemaphore(IGPUBinarySemaphore&&) noexcept = default;
        IGPUBinarySemaphore& operator=(IGPUBinarySemaphore&&) noexcept = default;
    };

    class IGPUTimelineSemaphore : public IGPUResource {
      public:
        ~IGPUTimelineSemaphore() override = 0;

        IGPUTimelineSemaphore(const IGPUTimelineSemaphore&) = delete;
        IGPUTimelineSemaphore& operator=(const IGPUTimelineSemaphore&) = delete;

        virtual void wait(uint64_t value, uint64_t timeout) = 0;
        virtual void signal(uint64_t value) = 0;
        virtual uint64_t getCurrentValue() const = 0;

      protected:
        IGPUTimelineSemaphore() = default;
        IGPUTimelineSemaphore(IGPUTimelineSemaphore&&) noexcept = default;
        IGPUTimelineSemaphore& operator=(IGPUTimelineSemaphore&&) noexcept = default;
    };
}  // namespace aetherion
