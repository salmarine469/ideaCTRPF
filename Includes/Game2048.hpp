#pragma once

#include <CTRPluginFramework.hpp>
#include <cmath>
#include "types.h"

namespace CTRPluginFramework
{
  class Game2048
  {
  public:
    void Game2048_Loop(void);
    static Game2048 &GetInstance(void)
    {
      static Game2048 instance;
      return instance;
    }

    Game2048(Game2048 const &) = delete;
    Game2048 &operator=(Game2048 const &) = delete;
    Game2048(Game2048 &&) = delete;
    Game2048 &operator=(Game2048 &&) = delete;

  private:
    static constexpr u8 FIELD_WIDTH = 4;
    static constexpr u8 BLOCK_WIDTH = 50;

    u64 _score = 0;
    std::vector<u8> _field = {std::vector<u8>(FIELD_WIDTH * FIELD_WIDTH, 0)};
    const Screen &topScr = OSD::GetTopScreen();
    const Screen &btmScr = OSD::GetBottomScreen();

    bool _isOpened = false;

    void DrawField(void);
    void MoveField(u8 direction);
    bool CheckFinish(void);
    void ResetGame(void);

    Game2048() = default;
    ~Game2048() = default;
  };
}