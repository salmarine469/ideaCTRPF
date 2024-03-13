#include "Command.hpp"

namespace CTRPluginFramework {
void Command::Command_Loop() {
  std::string commandLine_buffer = Color::Green << "command" << Color::White;
  Directory dir("");

  while (1) {
    if (dir.GetFullName().substr(dir.GetFullName().length() - 1, 1) == "/")
      Directory::Open(dir, dir.GetFullName().substr(0, dir.GetFullName().length() - 1));
    std::vector<std::string> files, folders;
    dir.ListFiles(files);
    dir.ListDirectories(folders);
    commandLine_buffer += "\n->";
    std::string input;
    u8 count = GetReturnCount(commandLine_buffer);

    Keyboard key(11 < count ? commandLine_buffer.substr(FindAll(commandLine_buffer, "\n")[count - 12] + 1) : commandLine_buffer);
    if (key.Open(input) <= -1)
      break;
    std::string space_delimiter = " ";
    input += space_delimiter;
    commandLine_buffer += input + "\n";
    std::vector<std::string> args{};

    size_t pos = 0;
    while ((pos = input.find(space_delimiter)) != std::string::npos) {
      args.emplace_back(input.substr(0, pos));
      input.erase(0, pos + space_delimiter.length());
    }
    if (args[0] == "ls") {
      size_t buff_size = commandLine_buffer.size();
      for (auto &&file : files)
        commandLine_buffer += "\n" + file;
      for (auto &&folder : folders)
        commandLine_buffer += Color::Green << "\n" + folder;
      if (files.size() + folders.size() > 10)
        MessageBox(commandLine_buffer.substr(buff_size) + "")();
      commandLine_buffer = commandLine_buffer << Color::White;
    } else if (args[0] == "exit")
      break;
    else if ((args[0] == "clear") || (args[0] == "cls"))
      commandLine_buffer = Color::Green << "command" << Color::White;
    else if (args[0] == "cd") {
      if (args.size() == 1)
        commandLine_buffer += "enter directory name";
      else
        Cd(dir, args[1], commandLine_buffer);
    } else if (args[0] == "pwd")
      commandLine_buffer += dir.GetFullName();
    else if (args[0] == "help") {
      if (args.size() == 1) {
        std::string _help = "command list\n・ls\n・clear|cls\n・cd\n・pwd\n・mkdir\n・tree\n・secret\n・cat\n・echo\n・rm\n・touch\n・entry\n・commit\n・exit\n・help [command]\n" + (Color::Green << "Can be done even when files and directories are ambiguous." << Color::White);
        commandLine_buffer += _help;
        MessageBox(_help + "")();
      } else if (args[1] == "ls")
        commandLine_buffer += "show files and folders";
      else if ((args[1] == "cls") || (args[1] == "clear"))
        commandLine_buffer += "clear the screen";
      else if (args[1] == "cd")
        commandLine_buffer += "change directory\ncd [directory]";
      else if (args[1] == "pwd")
        commandLine_buffer += "show current directory";
      else if (args[1] == "mkdir")
        commandLine_buffer += "make directory\nmkdir [directory]";
      else if (args[1] == "tree")
        commandLine_buffer += "show all files and directories like tree";
      else if (args[1] == "cat")
        commandLine_buffer += "display in binary\ncat [file]";
      else if (args[1] == "echo")
        commandLine_buffer += "return word\necho [word]";
      else if (args[1] == "rm")
        commandLine_buffer += "remove file or folder\nrm [file|folder]";
      else if (args[1] == "touch")
        commandLine_buffer += "make file\ntouch [file]";
      else if (args[1] == "entry")
        commandLine_buffer += "enable or disable entry(only GameFunc)\nentry\n-l list entries\n-e [entry] enable entry\n-d [entry] disable entry";
      else if (args[1] == "exit")
        commandLine_buffer += "exit command";
      else if (args[1] == "commit")
        commandLine_buffer += "show commit hash";
    } else if (args[0] == "mkdir") {
      if (args.size() == 1)
        commandLine_buffer += "enter directory's name";
      else {
        dir.Create(dir.GetFullName() + "/" + args[1]);
        Directory::Open(dir, dir.GetFullName());
      }
    } else if (args[0] == "tree") {
      commandLine_buffer += TreeFolder(dir.GetFullName(), 0);
      MessageBox(TreeFolder(dir.GetFullName(), 0) + "")();
    } else if (args[0] == "kani") {
      commandLine_buffer += "(/)(;,,;)(/)\n>=<\nV=(° °)=V";
    } else if (args[0] == "commit") {
      commandLine_buffer += Utils::Format("%x", COMMIT_HASH);
    } else if (args[0] == "cat") {
      if (args.size() == 1)
        commandLine_buffer += "enter file's name";
      else {
        if (File::Exists(dir.GetFullName() + "/" + GetFileObscurely(dir, args[1]))) {
          File file(dir.GetFullName() + "/" + GetFileObscurely(dir, args[1]));
          std::string text = dir.GetFullName() + "/" + GetFileObscurely(dir, args[1]) + "\n" + Utils::Format("%d bytes", file.GetSize()) + "\n", out;
          while (1) {
            u8 buffer;
            file.Read((void *)&buffer, 1);
            text += Utils::Format("%02X", buffer);
            if (file.Tell() % 8 == 0)
              text += "\n";
            else if (file.Tell() % 4 == 0)
              text += " ";
            if (file.GetSize() == file.Tell())
              break;
          }
          if (FindAll(text, "\n").size() > 10)
            MessageBox(text + "")();
          commandLine_buffer += text;
        } else {
          commandLine_buffer += dir.GetFullName() + "/" + GetFileObscurely(dir, args[1]) + " not found";
        }
      }
    } else if (args[0] == "echo") {
      if (args.size() == 1)
        commandLine_buffer += "enter word";
      else
        commandLine_buffer += args[1];
    } else if (args[0] == "rm") {
      if (args.size() == 1)
        commandLine_buffer += "enter file's or directory's name";
      else {
        if (args[1].find("/") == std::string::npos) {
          if (Directory::IsExists(dir.GetFullName() + "/" + GetFolderObscurely(dir, args[1]))) {
            commandLine_buffer += "really delete " + dir.GetFullName() + "/" + GetFolderObscurely(dir, args[1]) + "?[y/n]:\n";
            u8 count = GetReturnCount(commandLine_buffer);
            std::string output;
            if (Keyboard(11 < count ? commandLine_buffer.substr(FindAll(commandLine_buffer, "\n")[count - 12] + 1) : commandLine_buffer).Open(output) <= -1)
              break;
            if (output == "y")
              Directory::Remove(dir.GetFullName() + "/" + GetFolderObscurely(dir, args[1]));
          } else if (File::Exists(dir.GetFullName() + "/" + GetFileObscurely(dir, args[1]))) {
            commandLine_buffer += "really delete " + dir.GetFullName() + "/" + GetFileObscurely(dir, args[1]) + "?[y/n]:\n";
            u8 count = GetReturnCount(commandLine_buffer);
            std::string output;
            if (Keyboard(11 < count ? commandLine_buffer.substr(FindAll(commandLine_buffer, "\n")[count - 12] + 1) : commandLine_buffer).Open(output) <= -1)
              break;
            if (output == "y")
              File::Remove(dir.GetFullName() + "/" + GetFileObscurely(dir, args[1]));
          } else
            commandLine_buffer += "no files found";
        } else
          commandLine_buffer += "Only files in this directory can be deleted";
        Directory::Open(dir, dir.GetFullName());
      }
    } else if (args[0] == "touch") {
      if (args.size() == 1)
        commandLine_buffer += "enter file's name";
      else {
        File::Create(dir.GetFullName() + "/" + args[1]);
        commandLine_buffer += dir.GetFullName() + "/" + args[1];
      }
    } else if (args[0] == "entry") {
      if (args.size() >= 2) {
        PluginMenu *menu = PluginMenu::GetRunningInstance();
        std::vector<CTRPluginFramework::MenuEntry *> entries;
        std::vector<CTRPluginFramework::MenuEntry *> menuEntries = menu->GetEntryList();
        for (auto &&menuEntry : menuEntries)
          if (menuEntry->GetMenuFunc() == nullptr)
            entries.emplace_back(menuEntry);
        std::vector<CTRPluginFramework::MenuFolder *> menuFolders = menu->GetFolderList();
        for (auto &&menuFolder : menuFolders)
          GetEntries(menuFolder, entries);
        if (args[1] == "-l") {
          std::string buff = "only GameFunc:";
          for (MenuEntry *_entry : entries)
            buff += "\n" + _entry->Name();
          MessageBox(buff + "")();
          commandLine_buffer += buff;
        } else if ((args[1] == "-e") || (args[1] == "-d")) {
          if (args.size() == 2)
            commandLine_buffer += "enter entry name";
          else {
            for (MenuEntry *_entry : entries)
              if (_entry->Name() == args[2]) {
                if (args[1] == "-e")
                  _entry->Enable();
                else
                  _entry->Disable();
                goto END_TOGGLE_ENTRY;
              }
            commandLine_buffer += "can't find that GameFunc";
            continue;
          END_TOGGLE_ENTRY:
            commandLine_buffer += args[1] == "-e" ? "enabled " + args[2] : "disabled " + args[2];
          }
        } else
          commandLine_buffer += "unknown argment";
      } else
        commandLine_buffer += "Please add an argument\n-l -e -d";
    } else
      commandLine_buffer += "unknown command";
  }
}

std::vector<int> Command::FindAll(const std::string &str, const std::string &subStr) {
  std::vector<int> result;

  int subStrSize = subStr.size();
  size_t pos = str.find(subStr);

  while (pos != std::string::npos) {
    result.emplace_back(pos);
    pos = str.find(subStr, pos + subStrSize);
  }

  return result;
}

std::string Command::TreeFolder(const std::string &path, const u8 &space) {
  std::string buff = "";
  std::vector<std::string> files, folders;
  Directory dir(path);
  dir.ListFiles(files);
  dir.ListDirectories(folders);
  for (auto &&file : files)
    buff += std::string(space * 2, ' ') + "├" + file + "\n";
  for (auto &&folder : folders) {
    buff += std::string(space * 2, ' ') + "├" + (Color::Green << folder) + "\n"
            << Color::White;
    buff += TreeFolder(path + "/" + folder, space + 1);
  }
  return buff;
}

std::string Command::GetFileObscurely(const Directory &dir, const std::string &word) {
  std::vector<std::string> files;
  dir.ListFiles(files);
  for (auto &&file : files)
    if (file.find(word) != std::string::npos)
      return file;
  return "no file";
}

std::string Command::GetFolderObscurely(const Directory &dir, const std::string &word) {
  std::vector<std::string> folders;
  dir.ListDirectories(folders);
  for (auto &&folder : folders)
    if (folder.find(word) != std::string::npos)
      return folder;
  return "no folder";
}

size_t Command::GetReturnCount(std::string &str) {
  size_t count = 0;
  count = FindAll(str, "\n").size();
  for (size_t i = 0; i < count - 1; i++) {
    if (
        OSD::GetTextWidth(true, str.substr(FindAll(str, "\n")[i], FindAll(str, "\n")[i + 1] - FindAll(str, "\n")[i])) > 330)
      for (int j = 0;; j++) {
        if (OSD::GetTextWidth(true, str.substr(FindAll(str, "\n")[i], j)) > 330) {
          str = str.substr(0, FindAll(str, "\n")[i] + j - 1) + "\n" + str.substr(FindAll(str, "\n")[i] + j - 1);
          break;
        }
      }
  }
  count = FindAll(str, "\n").size();
  return count;
}

void Command::Cd(Directory &dir, std::string &path, std::string &str) {
  u64 pos = 18446744073709551615UL;

  if (path[0] == '.' && path[1] == '/')
    path = path.substr(1);

  if (path[0] == '/')
    path = path.substr(1);

  if (path.substr(0, 2) == "..") {
    Directory::Open(dir, dir.GetFullName().substr(0, FindAll(dir.GetFullName(), "/")[FindAll(dir.GetFullName(), "/").size() - 1]));
    if (path.find("/") == std::string::npos)
      str += dir.GetFullName() + "\n";
    else {
      path.erase(0, 3);
      std::vector<std::string> files, folders;
      dir.ListFiles(files);
      dir.ListDirectories(folders);
      Cd(dir, path, str);
    }
  } else if (Directory::IsExists(dir.GetFullName() + "/" + GetFolderObscurely(dir, path.find("/") != std::string::npos ? path.substr(0, path.find("/")) : path))) {
    Directory::Open(dir, dir.GetFullName() + "/" + GetFolderObscurely(dir, path.find("/") != std::string::npos ? path.substr(0, path.find("/")) : path));
    if ((pos = path.find("/")) == std::string::npos)
      str += dir.GetFullName() + "\n";
    else {
      path.erase(0, pos + 1);
      std::vector<std::string> files, folders;
      dir.ListFiles(files);
      dir.ListDirectories(folders);
      Cd(dir, path, str);
    }
  } else
    str += "invalid directory\n" + path;
}

void Command::GetEntries(const MenuFolder *folder, std::vector<MenuEntry *> &entries) {
  std::vector<MenuEntry *> menuEntries = folder->GetEntryList();
  for (auto &&menuEntry : menuEntries)
    if (menuEntry->GetMenuFunc() == nullptr)
      entries.emplace_back(menuEntry);
  std::vector<MenuFolder *> menuFolders = folder->GetFolderList();
  for (auto &&menuFolder : menuFolders)
    GetEntries(menuFolder, entries);
}
}  // namespace CTRPluginFramework