#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include "doptions/utils.hpp"
#ifndef DOPTIONS_EXCEPTIONS_HPP
#define DOPTIONS_EXCEPTIONS_HPP

namespace doptions {

class DOptionsException : public std::invalid_argument {
 public:
  explicit DOptionsException(std::string_view msg)
      : std::invalid_argument(std::string(msg)) {}
};

class ParseException : public DOptionsException {
 public:
  static auto unknownArg(std::string_view arg) -> ParseException {
    return ParseException("Unknown argument: " + std::string(arg));
  }

  static auto insufficientValues(std::string_view arg) -> ParseException {
    return ParseException("Insufficient values for arg: " + std::string(arg));
  }

  static auto multiArg(std::string_view arg) -> ParseException {
    return ParseException("Same argument appears multiple times: " +
                          std::string(arg));
  }

  template <typename T>
    requires(concepts::IsUnsignedInteger<T>)
  static auto outOfRange(size_t val) -> ParseException {
    auto [min, max] = NumberUtils::getLimits<T>();
    auto minStr = NumberUtils::toString(min);
    auto maxStr = NumberUtils::toString(max);
    auto valStr = NumberUtils::toString(val);
    return ParseException("Value out of range: " + valStr + " (" + minStr +
                          " - " + maxStr + ")");
  }

  template <typename T>
    requires(concepts::IsSignedInteger<T>)
  static auto outOfRange(uint64_t val) -> ParseException {
    auto [min, max] = NumberUtils::getLimits<T>();
    auto minStr = NumberUtils::toString(min);
    auto maxStr = NumberUtils::toString(max);
    auto valStr = NumberUtils::toString(val);
    return ParseException("Value out of range: " + valStr + " (" + minStr +
                          " - " + maxStr + ")");
  }

 private:
  explicit ParseException(std::string_view str)
      : DOptionsException("Parse Exception: " + std::string(str)) {};
};

class BuildException : public DOptionsException {
 public:
  static auto invalidName(std::string_view name) -> BuildException {
    return BuildException("Invalid name for argument: " + std::string(name));
  }

  static auto emptyName(std::string_view name) -> BuildException {
    return BuildException("Name cannot be empty: " + std::string(name));
  }

  static auto invalidSize(std::string_view name, uint8_t min, uint8_t max,
                          bool isShort) -> BuildException {
    auto size = std::to_string(name.size());
    std::string msg = isShort ? " [short: " : " [long: ";
    msg += NumberUtils::toString(min) + "-" + NumberUtils::toString(max);
    msg += "]";
    return BuildException("Name has invalid size: " + std::string(name) + " (" +
                          size + ")" + msg);
  }

 private:
  explicit BuildException(std::string_view str)
      : DOptionsException("Build Exception: " + std::string(str)) {};
};

}  // namespace doptions

#endif  // !DOPTIONS_EXCEPTIONS_HPP
