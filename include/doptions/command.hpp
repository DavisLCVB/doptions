#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include "doptions/exceptions.hpp"
#include "doptions/validations.hpp"
#ifndef DOPTIONS_COMMAND_HPP
#define DOPTIONS_COMMAND_HPP

#include <string>
#include "option.hpp"

namespace doptions {

class Command {
 public:
  static auto createCommand(const std::string& name)
      -> std::unique_ptr<Command> {
    NameValidations::validateName(name);
    NameValidations::validateSize(name, false);
    std::unique_ptr<Command> cmdPtr(new Command());
    cmdPtr->name_ = name;
    return cmdPtr;
  }

  template <typename T>
    requires(concepts::HasFromStr<T>)
  auto addOption(const std::string& name, T* var)
      -> std::unique_ptr<OptionBase>& {
    auto optPtr = Option<T>::createOption(name, var);
    options_.push_back(std::move(optPtr));
    return options_.at(options_.size() - 1);
  }

  [[nodiscard]] auto name() const -> const std::string& { return name_; }

  auto parseCommand(const std::vector<std::string>& args) -> void {
    std::map<std::string, size_t> nameMap;
    size_t idx = 0;
    for (const auto& opt : options_) {
      auto sName = opt->shortName();
      auto lName = opt->longName();
      if (!sName.empty()) {
        nameMap[sName] = idx;
      }
      if (!lName.empty()) {
        nameMap[lName] = idx;
      }
      idx++;
    }
    std::map<size_t, bool> parsed;
    size_t argsIndex = 0;
    for (; argsIndex < args.size(); ++argsIndex) {
      const auto& arg = args[argsIndex];
      if (!nameMap.contains(arg)) {
        throw ParseException::unknownArg(arg);
      }
      const auto argIdx = nameMap.at(arg);
      if (parsed.contains(argIdx)) {
        std::string argNames;
        for (const auto& [name, idx] : nameMap) {
          if (idx == argIdx) {
            argNames += name;
            argNames += ", ";
          }
        }
        throw ParseException::multiArg(argNames);
      }
      auto& opt = options_.at(argIdx);
      if (opt->needsValue()) {
        if (argsIndex + 1 >= args.size()) {
          throw ParseException::insufficientValues(arg);
        }
        opt->parseValue(args.at(argsIndex + 1));
        argsIndex++;
      } else {
        opt->parseValue("true");
      }
      parsed[argIdx] = true;
    }
  }

 private:
  Command() = default;
  std::vector<std::unique_ptr<OptionBase>> options_;
  std::string name_;
};

}  // namespace doptions

#endif  // !DOPTIONS_COMMAND_HPP
