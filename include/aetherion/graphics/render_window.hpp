#pragma once

#include "aetherion/platform/window.hpp"

namespace aetherion {
    class RenderWindow {
      public:
        RenderWindow(IWindow& window);
        ~RenderWindow() noexcept = default;

        RenderWindow(const RenderWindow&) = delete;
        RenderWindow& operator=(const RenderWindow&) = delete;

        RenderWindow(RenderWindow&&) = delete;
        RenderWindow& operator=(RenderWindow&&) = delete;

        IWindow& getWindow() { return window_; }
        const IWindow& getWindow() const { return window_; }

      private:
        IWindow& window_;
    };
}  // namespace aetherion