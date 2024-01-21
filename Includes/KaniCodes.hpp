#pragma once

#include <CTRPluginFramework.hpp>
#include <math.h>
#include "types.h"
#include "Helpers.hpp"
#include "csvc.h"
#include <curl/curl.h>

namespace CTRPluginFramework
{
  /*****************************************************/
  /*                                                   */
  /*             Generated by OSD Designer             */
  /*                  Made by Hidegon                  */
  /*  OSD Designer: https://HidegonSan.github.io/OSD/  */
  /*                                                   */
  /*****************************************************/
  void DrawLine(const Screen &scr, int srcX, int srcY, int dstX, int dstY, const Color &color);
  UIntVector GetStrPos(u32 posX, u32 posY, u32 borderWidth, u32 padding, int fontAlign);
  void DrawPlus(const Screen &scr, const std::string &str, u32 posX, u32 posY, u32 borderWidth, u32 padding, const Color &foreground, const Color &background, const Color &border, int fontAlign);
  void DrawSysfontPlus(const Screen &scr, const std::string &str, u32 posX, u32 posY, u32 borderWidth, u32 padding, const Color &foreground, const Color &background, const Color &border, bool fillBackground, bool rightAligned = false, int fontAlign = 0);
  float DegreeToRadian(float degree);
  void DrawCircle(const Screen &scr, u32 x, u32 y, u32 radiusStart, u32 radiusEnd, int start, int end, const Color &color, int origin);
  void DrawRectPlus(const Screen &scr, u32 posX, u32 posY, u32 width, u32 height, const Color &color, bool filled, int origin);
  void DrawDiamond(const Screen &scr, u16 leftX, u16 leftY, u16 rightX, u16 rightY, const Color &color, bool filled);

  bool TouchCircle(u32 x, u32 y, u8 size);
  bool TouchRect(u32 x, u32 y, u32 w, u32 h);

  void setScreenBuffer(u16 x, u16 y, Color color);
  void fillScreenBuffer(Color color);
  bool ShowScreenBuffer(const Screen &screen);

  std::string ReadSJIS(u32 Address);

  bool colorPicker(Color &out);

  void ForcePlay(void);

  bool PlaySound(const std::string &path);

  extern u8 *soundBuffer;
  constexpr u32 SOUND_BUFFER_ADDR = 0x7600000;
  constexpr u32 SOUND_BUFFER_SIZE = 0x100000;

  extern u8 *micBuffer;
  constexpr u32 MIC_BUFFER_ADDR = 0x7700000;
  constexpr u32 MIC_BUFFER_SIZE = 0x30000;
}
