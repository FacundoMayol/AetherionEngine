#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

namespace aetherion {
    template <typename T> struct Offset2D {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      "Offset2D<T>: T must be an integral or floating point type");
        T x{};
        T y{};
    };

    using Offset2Di = Offset2D<int32_t>;
    using Offset2Du = Offset2D<uint32_t>;
    using Offset2Df = Offset2D<float>;

    template <typename T> struct Extent2D {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      "Extent2D<T>: T must be an integral or floating point type");
        T width{};
        T height{};
    };

    using Extent2Di = Extent2D<int32_t>;
    using Extent2Du = Extent2D<uint32_t>;
    using Extent2Df = Extent2D<float>;

    template <typename T> struct Extent3D {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      "Extent3D<T>: T must be an integral or floating point type");
        T width{};
        T height{};
        T depth{};
    };

    using Extent3Di = Extent3D<int32_t>;
    using Extent3Du = Extent3D<uint32_t>;
    using Extent3Df = Extent3D<float>;

    template <typename T, typename U> struct Rect2D {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      "Rect2D<T>: T must be an integral or floating point type");
        static_assert(std::is_integral_v<U> || std::is_floating_point_v<U>,
                      "Rect2D<U>: U must be an integral or floating point type");
        Offset2D<T> offset;
        Extent2D<U> extent;
    };

    using Rect2Di = Rect2D<int32_t, uint32_t>;
    using Rect2Du = Rect2D<uint32_t, uint32_t>;
    using Rect2Df = Rect2D<float, float>;

    using Rect2Diu = Rect2D<int32_t, uint32_t>;

    using ColorValue = std::array<float, 4>;
}  // namespace aetherion