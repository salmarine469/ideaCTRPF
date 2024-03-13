#pragma once

#include <CTRPluginFramework.hpp>
extern "C" {
#include <lua5.1/lauxlib.h>
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
}
#include <map>
#include <set>

namespace CTRPluginFramework {
class LuaCTRPF {
 public:
  bool Run(int nargs, int nresults, int errfunc, bool while_game = false);
  // void Stop(void);
  LuaCTRPF(const std::string &code);
  ~LuaCTRPF(void);

 private:
  lua_State *_state;
  const std::string _code;
  // static std::set<std::string> _LuaRun;
  // static bool LuaRun(const Screen &screen);
  static int LMessageBox(lua_State *state);
  static int LNotify(lua_State *state);
  static int LInjectKey(lua_State *state);
};
}  // namespace CTRPluginFramework
