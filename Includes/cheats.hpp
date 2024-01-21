#ifndef CHEATS_H
#define CHEATS_H

#include <CTRPluginFramework.hpp>
#include "Helpers.hpp"
#include "Unicode.h"

namespace CTRPluginFramework
{
  enum BMP_SCREEN
  {
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
}
#endif