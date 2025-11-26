#pragma once
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "utils.hpp"
#ifndef DOPTIONS_APPLICATION_HPP
#define DOPTIONS_APPLICATION_HPP

#include "command.hpp"
#include "option.hpp"

namespace doptions {

class Application {
 public:
  static auto createApp() -> Application { return {}; }

  template <typename T>
    requires(concepts::HasFromStr<T>)
  auto addOption(const std::string& name, T* var)
      -> std::unique_ptr<OptionBase>& {
    auto optPtr = Option<T>::createOption(name, var);
    options_.push_back(std::move(optPtr));
    return options_.at(options_.size() - 1);
  }

  auto addCommand(const std::string& name, bool* var)
      -> std::unique_ptr<Command>& {
    auto cmdPtr = Command::createCommand(name);
    commands_.emplace_back(std::move(cmdPtr), var);
    return commands_.at(commands_.size() - 1).first;
  }

  auto parse(int32_t argc, char** argv) -> void {
    auto args = buildArray(argc, argv);
    auto [optionMap, commandMap] = buildReconMaps();
    std::map<size_t, bool> parsedOptions;

    for (size_t idx = 0; idx < args.size(); ++idx) {
      const auto& arg = args[idx];

      if (commandMap.contains(arg)) {
        processCommand(commandMap.at(arg), args, idx);
        break;
      }

      if (optionMap.contains(arg)) {
        processOption(args, optionMap, optionMap.at(arg), idx, parsedOptions,
                      arg);
      } else {
        throw ParseException::unknownArg(arg);
      }
    }
  }

 private:
  static auto buildArray(int32_t argc, char** argv)
      -> std::vector<std::string> {
    std::vector<std::string> vec;
    vec.reserve(argc - 1);
    for (int32_t idx = 1; idx < argc; ++idx) {
      vec.push_back(std::string(argv[idx]));  // NOLINT
    }
    return vec;
  }

  static auto buildDuplicateArgNames(
      const std::map<std::string, size_t>& nameMap, size_t targetIdx)
      -> std::string {
    std::string argNames;
    for (const auto& [name, index] : nameMap) {
      if (index == targetIdx) {
        argNames += name;
        argNames += ", ";
      }
    }
    return argNames;
  }

  auto processCommand(size_t cmdIdx, const std::vector<std::string>& args,
                      size_t startIdx) -> void {
    auto& [cmdPtr, executed] = commands_.at(cmdIdx);
    std::vector<std::string> cmdArgs(
        args.begin() + static_cast<std::ptrdiff_t>(startIdx + 1), args.end());
    cmdPtr->parseCommand(cmdArgs);
    *executed = true;
  }

  auto processOption(const std::vector<std::string>& args,
                     const std::map<std::string, size_t>& optionMap,
                     size_t optIdx, size_t& idx,
                     std::map<size_t, bool>& parsedOptions,
                     const std::string& arg) -> void {
    if (parsedOptions.contains(optIdx)) {
      throw ParseException::multiArg(buildDuplicateArgNames(optionMap, optIdx));
    }

    auto& opt = options_.at(optIdx);
    if (opt->needsValue()) {
      if (idx + 1 >= args.size()) {
        throw ParseException::insufficientValues(arg);
      }
      opt->parseValue(args.at(idx + 1));
      idx++;
    } else {
      opt->parseValue("true");
    }
    parsedOptions[optIdx] = true;
  }

  auto buildReconMaps() -> std::pair<std::map<std::string, size_t>,
                                     std::map<std::string, size_t>> {
    std::map<std::string, size_t> optionMap;
    std::map<std::string, size_t> commandMap;
    for (size_t idx = 0; idx < options_.size(); ++idx) {
      const auto& opt = options_[idx];
      auto sName = opt->shortName();
      auto lName = opt->longName();
      if (!sName.empty()) {
        optionMap[sName] = idx;
      }
      if (!lName.empty()) {
        optionMap[lName] = idx;
      }
    }
    for (size_t idx = 0; idx < commands_.size(); ++idx) {
      const auto& cmd = commands_[idx].first;
      commandMap[cmd->name()] = idx;
    }
    return {optionMap, commandMap};
  }

  std::vector<std::unique_ptr<OptionBase>> options_;
  std::vector<std::pair<std::unique_ptr<Command>, bool*>> commands_;
  Application() = default;
};

}  // namespace doptions

#endif  // !DOPTIONS_APPLICATION_HPP
