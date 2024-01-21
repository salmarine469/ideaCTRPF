#include "LuaCTRPF.hpp"

namespace CTRPluginFramework
{
  bool LuaCTRPF::Run(int nargs, int nresults, int errfunc, bool while_game)
  {
    // if (while_game)
    // {
    //   _LuaRun.insert(_code);
    //   OSD::Run(LuaRun);
    // }
    // else
      return !lua_pcall(_state, nargs, nresults, errfunc);
  }

  // void LuaCTRPF::Stop(void)
  // {
  //   auto it = _LuaRun.find(_code);
  //   if (it != _LuaRun.end())
  //     _LuaRun.erase(it);
  //   if (!_LuaRun.size())
  //     OSD::Stop(LuaRun);
  // }

  // bool LuaCTRPF::LuaRun(const Screen &screen)
  // {
  //   for (auto &&__LuaRun : _LuaRun)
  //     LuaCTRPF(__LuaRun).Run(0, 0, 0);
  //   return true;
  // }

  int LuaCTRPF::LMessageBox(lua_State *state)
  {
    MessageBox(lua_tostring(state, -1))();
    lua_pop(state, -1);
    return 0;
  }

  int LuaCTRPF::LNotify(lua_State *state)
  {
    OSD::Notify(lua_tostring(state, -1));
    lua_pop(state, -1);
    return 0;
  }
  int LuaCTRPF::LInjectKey(lua_State *state)
  {
    std::map<std::string, int> key_map = {
        {"A", 1},
        {"B", 1 << 1},
        {"Select", 1 << 2},
        {"Start", 1 << 3},
        {"DPadRight", 1 << 4},
        {"DPadLeft", 1 << 5},
        {"DPadUp", 1 << 6},
        {"DPadDown", 1 << 7},
        {"R", 1 << 8},
        {"L", 1 << 9},
        {"X", 1 << 10},
        {"Y", 1 << 11},
        {"ZL", 1 << 14},
        {"ZR", 1 << 15},
        {"Touchpad", 1 << 20},
        {"CStickRight", 1 << 24},
        {"CStickLeft", 1 << 25},
        {"CStickUp", 1 << 26},
        {"CStickDown", 1 << 27},
        {"CPadRight", 1 << 28},
        {"CPadLeft", 1 << 29},
        {"CPadUp", 1 << 30},
        {"CPadDown", 1 << 31},
        {"Up", DPadUp | CPadUp},
        {"Down", DPadDown | CPadDown},
        {"Left", DPadLeft | CPadLeft},
        {"Right", DPadRight | CPadRight},
        {"CPad", CPadLeft | CPadRight | CPadUp | CPadDown},
        {"CStick", CStickLeft | CStickRight | CStickUp | CStickDown}};
    Controller::InjectKey(key_map[lua_tostring(state, -1)]);
    lua_pop(state, -1);
    return 0;
  }

  LuaCTRPF::LuaCTRPF(const std::string &code)
  {
    _state = luaL_newstate();
    luaL_openlibs(_state);

    luaL_loadstring(_state, code.c_str());

    std::vector<lua_CFunction> functions = {LMessageBox, LNotify, LInjectKey};
    std::vector<std::string> function_names = {"LMessageBox", "LNotify", "LInjectKey"};
    for (size_t i = 0; i < functions.size(); i++)
    {
      lua_pushcfunction(_state, functions[i]);
      lua_setglobal(_state, function_names[i].c_str());
    }
  }

  LuaCTRPF::~LuaCTRPF()
  {
    lua_close(_state);
  }

  // std::set<std::string> LuaCTRPF::_LuaRun;
}