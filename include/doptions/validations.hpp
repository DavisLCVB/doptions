#pragma once
#include <cctype>
#include <string>
#include <string_view>
#include <vector>
#include "exceptions.hpp"
#ifndef DOPTIONS_VALIDATIONS_HPP
#define DOPTIONS_VALIDATIONS_HPP

#include <cstdint>

namespace doptions {

constexpr uint8_t defaultShortLimit = 3;
constexpr uint8_t defaultLongLimit = 100;

struct NameValidationConfig {
  uint8_t shortNameLimit{defaultShortLimit};
  uint8_t longNameLimit{defaultLongLimit};
  bool nameContainsDots{false};
  bool nameContainsDashes{true};
  bool nameContainsUnderscores{true};
  std::vector<std::string> reserverNames;
};

static NameValidationConfig defualtConfig = NameValidationConfig{};

class NameValidations {
 public:
  static auto setConfig(const NameValidationConfig& config) -> void {
    NameValidations::options = config;
  }

  static auto validateName(std::string_view name) -> void {
    if (name.empty()) {
      throw BuildException::emptyName("Argument name");
    }
    bool first{true};
    for (auto l : name) {
      if (first && !validChar(l, true)) {
        throw BuildException::invalidName(std::string(name));
      }
      if (first) {
        first = false;
      }
      if (!validChar(l)) {
        throw BuildException::invalidName(std::string(name));
      }
    }
  }

  static auto validChar(const char& ch, bool first = false) -> bool {
    if (first) {
      return std::isalpha(ch) != 0;
    }
    if (std::isalnum(ch) != 0) {
      return true;
    }
    if (options.nameContainsDashes && ch == '-') {
      return true;
    }
    if (options.nameContainsDots && ch == '.') {
      return true;
    }
    if (options.nameContainsUnderscores && ch == '_') {
      return true;
    }
    return false;
  }

  static void validateSize(std::string_view name, bool isShort) {
    const size_t size = name.size();
    if (isShort && (size > options.shortNameLimit || size == 0)) {
      throw BuildException::invalidSize(name, 0, options.shortNameLimit, true);
    }
    if (!isShort && (size <= options.shortNameLimit || size == 0 ||
                     size > options.longNameLimit)) {
      throw BuildException::invalidSize(name, options.shortNameLimit,
                                        options.longNameLimit, false);
    }
  }

 private:
  inline static NameValidationConfig options = NameValidationConfig();
};

}  // namespace doptions

#endif  // !DOPTIONS_VALIDATIONS_HPP
