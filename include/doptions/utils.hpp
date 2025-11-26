#pragma once
#include <cctype>
#include <concepts>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#ifndef DOPTIONS_UTILS_HPP
#define DOPTIONS_UTILS_HPP

namespace doptions {
class StringUtils {
 public:
  static auto trim(std::string_view str) -> std::string_view {
    if (str.empty()) {
      return str;
    }

    size_t sIndex = 0;
    size_t eIndex = str.size() - 1;

    while (sIndex < str.size() && std::isspace(str.at(sIndex)) != 0) {
      ++sIndex;
    }

    if (sIndex == str.size()) {
      return {};
    }

    while (eIndex > sIndex && std::isspace(str.at(eIndex)) != 0) {
      --eIndex;
    }

    return str.substr(sIndex, eIndex - sIndex + 1);
  }
};

namespace concepts {
template <typename T>
concept IsUInt8 = std::same_as<std::remove_cvref_t<T>, uint8_t>;

template <typename T>
concept IsUInt16 = std::same_as<std::remove_cvref_t<T>, uint16_t>;

template <typename T>
concept IsUInt32 = std::same_as<std::remove_cvref_t<T>, uint32_t>;

template <typename T>
concept IsUInt64 = std::same_as<std::remove_cvref_t<T>, uint64_t>;

template <typename T>
concept IsInt8 = std::same_as<std::remove_cvref_t<T>, int8_t>;

template <typename T>
concept IsInt16 = std::same_as<std::remove_cvref_t<T>, int16_t>;

template <typename T>
concept IsInt32 = std::same_as<std::remove_cvref_t<T>, int32_t>;

template <typename T>
concept IsInt64 = std::same_as<std::remove_cvref_t<T>, int64_t>;

template <typename T>
concept IsSignedInteger = IsInt8<T> || IsInt16<T> || IsInt32<T> || IsInt64<T>;

template <typename T>
concept IsUnsignedInteger =
    IsUInt8<T> || IsUInt16<T> || IsUInt32<T> || IsUInt64<T>;

template <typename T>
concept IsInteger = IsSignedInteger<T> || IsUnsignedInteger<T>;

template <typename T>
concept isFloat = std::is_floating_point_v<T>;

template <typename T>
struct HasFromStrT : std::false_type {};

template <typename T>
  requires std::is_fundamental_v<T>
struct HasFromStrT<T> : std::true_type {};

template <>
struct HasFromStrT<std::string> : std::true_type {};

template <typename V>
concept HasFromStr = HasFromStrT<V>::value;

}  // namespace concepts

class NumberUtils {
 public:
  template <typename T>
    requires(concepts::IsInteger<T>)
  static auto getLimits() -> std::pair<T, T> {
    return {std::numeric_limits<T>::min(), std::numeric_limits<T>::max()};
  }

  template <typename T>
    requires(concepts::IsSignedInteger<T>)
  static auto intToString(T val) -> std::string {
    if constexpr (concepts::IsInt8<T>) {
      return std::to_string(static_cast<int32_t>(val));
    }
    return std::to_string(val);
  }

  template <typename T>
    requires(concepts::IsUnsignedInteger<T>)
  static auto uIntToString(T val) -> std::string {
    if constexpr (concepts::IsUInt8<T>) {
      return std::to_string(static_cast<uint32_t>(val));
    }
    return std::to_string(val);
  }

  template <typename T>
    requires(concepts::isFloat<T>)
  static auto floatToString(T val) -> std::string {
    return std::to_string(val);
  }

  template <typename T>
    requires(concepts::IsInteger<T> || concepts::isFloat<T>)
  static auto toString(T val) -> std::string {
    if constexpr (concepts::IsSignedInteger<T>) {
      return intToString(val);
    } else if constexpr (concepts::IsUnsignedInteger<T>) {
      return uIntToString(val);
    } else if constexpr (concepts::isFloat<T>) {
      return floatToString(val);
    }
  }
};

}  // namespace doptions

#endif  // !DOPTIONS_UTILS_HPP
