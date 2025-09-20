#pragma once

#include <cstdint>
#include <type_traits>

namespace aetherion {
    using FlagType = uint32_t;
    using Flag64Type = uint64_t;

    template <typename Enum> class EnumFlags {
        static_assert(std::is_enum_v<Enum>, "EnumFlags requires an enum type");

        using Underlying = std::underlying_type_t<Enum>;

      public:
        constexpr EnumFlags() noexcept : bits_(0) {}
        constexpr EnumFlags(Enum flag) noexcept : bits_(static_cast<Underlying>(flag)) {}
        constexpr EnumFlags(const EnumFlags&) noexcept = default;

        constexpr EnumFlags operator|(Enum flag) const noexcept {
            return EnumFlags(bits_ | static_cast<Underlying>(flag));
        }

        constexpr EnumFlags operator|(EnumFlags other) const noexcept {
            return EnumFlags(bits_ | other.bits_);
        }

        EnumFlags& operator|=(Enum flag) noexcept {
            bits_ |= static_cast<Underlying>(flag);
            return *this;
        }

        // Bitwise AND
        constexpr EnumFlags operator&(Enum flag) const noexcept {
            return EnumFlags(bits_ & static_cast<Underlying>(flag));
        }

        constexpr EnumFlags operator&(EnumFlags other) const noexcept {
            return EnumFlags(bits_ & other.bits_);
        }

        EnumFlags& operator&=(Enum flag) noexcept {
            bits_ &= static_cast<Underlying>(flag);
            return *this;
        }

        constexpr EnumFlags operator~() const noexcept { return EnumFlags(~bits_); }

        constexpr bool contains(Enum flag) const noexcept {
            return (bits_ & static_cast<Underlying>(flag)) != 0;
        }

        constexpr explicit operator bool() const noexcept { return bits_ != 0; }

      private:
        explicit constexpr EnumFlags(Underlying bits_) noexcept : bits_(bits_) {}

        Underlying bits_;
    };

    template <typename Enum> struct FlagTraits {
        static constexpr bool isBitmask = false;
    };

    template <typename Enum, typename = typename std::enable_if<FlagTraits<Enum>::isBitmask>::type>
    constexpr EnumFlags<Enum> operator|(Enum lhs, Enum rhs) {
        return EnumFlags<Enum>(lhs) | EnumFlags<Enum>(rhs);
    }

    template <typename Enum, typename = typename std::enable_if<FlagTraits<Enum>::isBitmask>::type>
    constexpr EnumFlags<Enum> operator&(Enum lhs, Enum rhs) {
        return EnumFlags<Enum>(lhs) & EnumFlags<Enum>(rhs);
    }

    template <typename Enum, typename = typename std::enable_if<FlagTraits<Enum>::isBitmask>::type>
    constexpr EnumFlags<Enum> operator^(Enum lhs, Enum rhs) {
        return EnumFlags<Enum>(lhs) ^ EnumFlags<Enum>(rhs);
    }

    template <typename Enum, typename = typename std::enable_if<FlagTraits<Enum>::isBitmask>::type>
    constexpr EnumFlags<Enum> operator~(Enum bit) {
        using MaskType = typename std::underlying_type<Enum>::type;
        return EnumFlags<Enum>(~static_cast<MaskType>(bit));
    }

#define DECLARE_FLAG_ENUM(Enum)                 \
    template <> struct FlagTraits<Enum> {       \
        static constexpr bool isBitmask = true; \
    };                                          \
    using Enum##Flags = EnumFlags<Enum>;
}  // namespace aetherion