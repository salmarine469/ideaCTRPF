#pragma once

#include <CTRPluginFramework.hpp>
#include <clocale>
#include <cmath>
#include <cstdlib>
#include <numbers>

#include "Convert.hpp"
#include "OSDPlus.hpp"
#include "csvc.h"

namespace CTRPluginFramework {
bool TouchRect(u32 x, u32 y, u32 w, u32 h);
bool TouchRect(u32 x, u32 y, u32 w, u32 h, UIntVector &touchPos);
bool TouchCircle(u32 x, u32 y, u8 radius);
bool TouchCircle(u32 x, u32 y, u8 radius, UIntVector &touchPos);

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
}  // namespace CTRPluginFramework
