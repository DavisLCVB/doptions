#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include "doptions/exceptions.hpp"
#include "doptions/utils.hpp"
#include "doptions/validations.hpp"
#ifndef DOPTIONS_OPTION_HPiP
#define DOPTIONS_OPTION_HPP

namespace doptions {

template <typename V>
inline auto fromStr(const std::string&) -> V;

template <concepts::IsSignedInteger T>
inline auto fromStr(const std::string& str) -> T {
  auto value = std::stoll(str);
  auto [min, max] = NumberUtils::getLimits<T>();
  if (value < min || value > max) {
    throw ParseException::outOfRange<T>(value);
  }
  return static_cast<T>(value);
}

template <concepts::IsUnsignedInteger T>
inline auto fromStr(const std::string& str) -> T {
  auto value = std::stoull(str);
  auto [min, max] = NumberUtils::getLimits<T>();
  if (value < min || value > max) {
    throw ParseException::outOfRange<T>(value);
  }
  return static_cast<T>(value);
}

template <>
inline auto fromStr<bool>(const std::string& str) -> bool {
  return str == "true";
}

template <>
inline auto fromStr<float>(const std::string& str) -> float {
  return std::stof(str);
}

template <>
inline auto fromStr<double>(const std::string& str) -> double {
  return std::stod(str);
}

template <>
inline auto fromStr<std::string>(const std::string& str) -> std::string {
  return str;
}

//NOLINTNEXTLINE
#define REGISTER_TYPE(Type)                                         \
  template <>                                                       \
  struct doptions::concepts::HasFromStrT<Type> : std::true_type {}; \
  template <>                                                       \
  auto doptions::fromStr<Type>(const std::string& str) -> Type

class OptionBase {
 public:
  OptionBase(const OptionBase&) = default;
  OptionBase(OptionBase&&) = delete;
  auto operator=(const OptionBase&) -> OptionBase& = default;
  auto operator=(OptionBase&&) -> OptionBase& = delete;
  virtual ~OptionBase() = default;
  [[nodiscard]] virtual auto shortName() const -> const std::string& = 0;
  [[nodiscard]] virtual auto longName() const -> const std::string& = 0;
  [[nodiscard]] virtual auto needsValue() const -> bool = 0;
  virtual auto parseValue(const std::string& str) -> void = 0;

 protected:
  OptionBase() = default;
};

template <typename V>
  requires(concepts::HasFromStr<V>)
class Option : public OptionBase {
 public:
  static auto createOption(const std::string& name, V* var)
      -> std::unique_ptr<Option> {
    auto [shortName, longName] = validateName(name);
    std::unique_ptr<Option> opt;
    opt.reset(new Option());
    opt->shortName_ = std::string(shortName);
    opt->longName_ = std::string(longName);
    if (!opt->shortName_.empty()) {
      opt->shortName_.insert(0, 1, '-');
    }
    if (!opt->longName_.empty()) {
      opt->longName_.insert(0, "--");
    }
    opt->value_ = var;
    if constexpr (std::is_same_v<V, bool>) {
      opt->needsValue_ = false;
    }
    return opt;
  }

  [[nodiscard]] auto needsValue() const -> bool override { return needsValue_; }

  [[nodiscard]] auto shortName() const -> const std::string& override {
    return this->shortName_;
  }

  [[nodiscard]] auto longName() const -> const std::string& override {
    return this->longName_;
  }

  void parseValue(const std::string& str) override {
    V valV = fromStr<V>(str);
    *value_ = valV;
  }

 private:
  static auto validateName(const std::string& name)
      -> std::pair<std::string_view, std::string_view> {
    if (name.empty()) {
      throw BuildException::emptyName("Argument Name");
    }
    std::string_view data = name;
    data = StringUtils::trim(data);
    if (data.empty()) {
      throw BuildException::emptyName("Argument Name");
    }
    const size_t separator = data.find(',');
    if (separator != std::string::npos) {
      std::string_view shortName = data.substr(0, separator);
      std::string_view longName = data.substr(separator + 1);
      shortName = StringUtils::trim(shortName);
      longName = StringUtils::trim(longName);
      if (shortName.empty()) {
        throw BuildException::emptyName("Short Name");
      }
      if (longName.empty()) {
        throw BuildException::emptyName("Long name");
      }
      if (shortName.at(0) == '-') {
        shortName = shortName.substr(1);
      }
      if (longName.starts_with("--")) {
        longName = longName.substr(2);
      }
      NameValidations::validateName(shortName);
      NameValidations::validateName(longName);
      NameValidations::validateSize(shortName, true);
      NameValidations::validateSize(longName, false);
      return {shortName, longName};
    }
    bool isShort = false;
    if (data.starts_with("--") && data.size() >= 3) {
      data = data.substr(2);
      isShort = false;
    } else if (data.at(0) == '-' && data.size() >= 2) {
      data = data.substr(1);
      isShort = true;
    } else {
      isShort = data.size() <= 3;
    }
    data = StringUtils::trim(data);
    NameValidations::validateSize(data, isShort);
    NameValidations::validateName(data);
    if (isShort) {
      return {data, {}};
    }
    return {{}, data};
  };

  static const size_t shortNameLimit;
  static const size_t longNameLimit;

  Option() = default;
  bool needsValue_{true};
  std::string shortName_;
  std::string longName_;
  V* value_;
};

template <typename T>
  requires(concepts::HasFromStr<T>)
const size_t Option<T>::shortNameLimit = 3;

template <typename T>
  requires(concepts::HasFromStr<T>)
const size_t Option<T>::longNameLimit = 100;

}  // namespace doptions

#endif  // GDOPTIONS_OPTION_HPP
