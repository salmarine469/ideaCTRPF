#pragma once

#include <CTRPluginFramework.hpp>
#include <bitset>
#include "types.h"

namespace CTRPluginFramework
{
  class LifeGame
  {
  public:
    void LifeGame_Loop(void);
    static LifeGame &GetInstance(void)
    {
      static LifeGame instance;
      return instance;
    }

    LifeGame(LifeGame const &) = delete;
    LifeGame &operator=(LifeGame const &) = delete;
    LifeGame(LifeGame &&) = delete;
    LifeGame &operator=(LifeGame &&) = delete;

  private:
    static constexpr u8 FIELD_WIDTH = 100;
    static constexpr u8 FIELD_HEIGHT = 60;
    static constexpr u8 BLOCK_WIDTH = 16;

    UIntVector _selector = {4, 6};
    UIntVector _btmPos = {3, 5};
    bool _isLoopingGen = false;
    int _speed = 200;
    bool _isTorus = true;

    std::bitset<FIELD_HEIGHT * FIELD_WIDTH> _field;

    const Screen &topScr = OSD::GetTopScreen();
    const Screen &btmScr = OSD::GetBottomScreen();

    void DrawField(void);
    void NextGen(void);
    s8 LivesAround(s16 x, s16 y);

    LifeGame() = default;
    ~LifeGame() = default;
  };
}