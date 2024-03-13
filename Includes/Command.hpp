#pragma once

#include "Convert.hpp"
#include "JPKeyboard.hpp"

namespace CTRPluginFramework {
class Command {
 public:
  void Command_Loop();
  static Command &GetInstance() {
    static Command instance;
    return instance;
  }

  Command(Command const &) = delete;
  Command &operator=(Command const &) = delete;
  Command(Command &&) = delete;
  Command &operator=(Command &&) = delete;

 private:
  std::vector<int> FindAll(const std::string &str, const std::string &subStr);
  std::string TreeFolder(const std::string &path, const u8 &space);
  std::string GetFileObscurely(const Directory &dir, const std::string &word);
  std::string GetFolderObscurely(const Directory &dir, const std::string &word);
  size_t GetReturnCount(std::string &str);
  void Cd(Directory &dir, std::string &path, std::string &str);
  void GetEntries(const MenuFolder *folder, std::vector<MenuEntry *> &entries);

  Command() = default;
  ~Command() = default;
};
}  // namespace CTRPluginFramework
