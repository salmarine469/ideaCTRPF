#pragma once

#include <CTRPluginFramework.hpp>
#include "types.h"

namespace CTRPluginFramework
{
  class threeDGame
  {
  public:
    void threeDGameLoop(void);


    static threeDGame &GetInstance()
    {
      static threeDGame instance;
      return instance;
    }
    
    threeDGame(threeDGame const &) = delete;
    threeDGame &operator=(threeDGame const &) = delete;
    threeDGame(threeDGame &&) = delete;
    threeDGame &operator=(threeDGame &&) = delete;
  private:
    u8 _field_width = 8;
    u8 _field_height = 8;
    u8 _block_width = 20;
    u8 _block_height = 6;
    const Color _blockColors[9] = {Color::Lime, Color::LimeGreen, Color::Green, Color::SkyBlue, Color::Blue,Color::Purple,Color::Yellow,Color::Orange,Color::Red};
    std::vector<u8> _field = std::vector<u8>(_field_width * _field_height, 0);
    FloatVector _pos = {0.9,0.9};
    bool _isOpened = false;

    void DrawScreen(void);

    threeDGame();
    ~threeDGame() = default;
  };
}