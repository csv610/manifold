#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>

struct Args {
  std::map<std::string, std::string> opts;
  std::vector<std::string> positional;
  size_t idx = 0;

  Args() = default;

  static Args parse(int argc, char* argv[]) {
    Args args;
    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      bool isFlag = arg.size() >= 2 && arg[0] == '-' && arg[1] != '-';
      if (isFlag || arg.rfind("--", 0) == 0) {
        std::string key = (arg.rfind("--", 0) == 0) ? arg.substr(2) : arg.substr(1);
        if (i + 1 < argc && argv[i + 1][0] != '-') {
          args.opts[key] = argv[++i];
        } else {
          args.opts[key] = "true";
        }
      } else if (!arg.empty() && arg.find('=') != std::string::npos) {
        size_t eq = arg.find('=');
        args.opts[arg.substr(1, eq - 1)] = arg.substr(eq + 1);
      } else {
        args.positional.push_back(arg);
      }
    }
    return args;
  }

  bool has(const std::string& flag) const { return opts.count(flag); }
  std::string get(const std::string& flag, const std::string& def = "") const {
    auto it = opts.find(flag);
    return it != opts.end() ? it->second : def;
  }
  double getf(const std::string& flag, double def = 0.0) const {
    auto it = opts.find(flag);
    return it != opts.end() ? std::stod(it->second) : def;
  }
  int geti(const std::string& flag, int def = 0) const {
    auto it = opts.find(flag);
    return it != opts.end() ? std::stoi(it->second) : def;
  }
  std::string pop(size_t i = 0) {
    if (idx + i < positional.size()) return positional[idx + i];
    return "";
  }
  std::string next() { return positional.size() > idx ? positional[idx++] : ""; }
  size_t count() const { return positional.size() > idx ? positional.size() - idx : 0; }
  size_t remaining() const { return positional.size() - idx; }
};