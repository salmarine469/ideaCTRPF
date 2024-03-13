#pragma once

#include <arpa/inet.h>

#include "Command.hpp"
#include "Convert.hpp"
#include "Curl.hpp"
#include "Game2048.hpp"
#include "JPKeyboard.hpp"
#include "KaniCodes.hpp"
#include "Led.hpp"
#include "LifeGame.hpp"
#include "LuaCTRPF.hpp"
#include "Paint.hpp"
#include "Tetris.hpp"
#include "Unicode.h"
#include "ncsnd.h"
#include "osdjp.hpp"
#include "threeDGame.hpp"
#include "ButtonEx.hpp"

namespace CTRPluginFramework {
enum BMP_SCREEN {
  BMP_TOP,
  BMP_BOTTOM
};

void Test1(MenuEntry *entry);
void Pipes(MenuEntry *entry);
void Search(MenuEntry *entry);
void Cube(MenuEntry *entry);
void BadApple(MenuEntry *entry);
void JPNotify(MenuEntry *entry);
void PlayMusic(MenuEntry *entry);
void Command(MenuEntry *entry);
void ColorPicker(MenuEntry *entry);
void Tetris(MenuEntry *entry);
void MenuEntryPaint(MenuEntry *entry);
void LifeGame(MenuEntry *entry);
void threeD(MenuEntry *entry);
void LedEffect(MenuEntry *entry);
void Game2048(MenuEntry *entry);
void Projection(MenuEntry *entry);
void SendDiscord(MenuEntry *entry);
void Update3gx(MenuEntry *entry);
void ParrotingBack(MenuEntry *entry);

std::string getFilePath();
void autoPatchCode(MenuEntry *entry);

std::vector<u64> getFrame(int &frame);
}  // namespace CTRPluginFramework
