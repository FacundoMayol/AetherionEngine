#pragma once

#include <optional>
#include <type_traits>
#include <utility>

namespace aetherion {
    template <typename ResultCode, typename Value>
        requires std::is_enum_v<ResultCode>
    class ResultValue {
      public:
        ResultValue(const Value& value, ResultCode code = ResultCode{})
            : value_(value), code_(code) {}

        ResultValue(Value&& value, ResultCode code = ResultCode{})
            : value_(std::move(value)), code_(code) {}

        explicit ResultValue(ResultCode code) : value_(std::nullopt), code_(code) {}

        [[nodiscard]] bool has_value() const noexcept { return value_.has_value(); }
        [[nodiscard]] explicit operator bool() const noexcept { return value_.has_value(); }

        [[nodiscard]] Value& value() { return value_.value(); }
        [[nodiscard]] const Value& value() const { return value_.value(); }

        [[nodiscard]] ResultCode code() const { return code_; }

        template <typename U> [[nodiscard]] Value value_or(U&& default_value) const {
            return value_.value_or(std::forward<U>(default_value));
        }

        bool operator==(const ResultValue& other) const {
            return value_ == other.value_ && code_ == other.code_;
        }
        bool operator!=(const ResultValue& other) const { return !(*this == other); }

      private:
        std::optional<Value> value_;
        ResultCode code_;
    };
}  // namespace aetherion