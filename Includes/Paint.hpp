#pragma once

#include "KaniCodes.hpp"

namespace CTRPluginFramework {
using PaintContext = std::vector<std::vector<Color>>;

class Paint {
 public:
  struct Pen {
    Color color;
    u32 size;
  };

  enum Mode {
    PEN,
    ERASER,
    BUCKET
  };

  Paint();

  int Open(void);

 private:
  PaintContext _ctx;
  const Screen *_screen;
  Pen _pen;

  void _drawLine(int srcX, int srcY, int dstX, int dstY);
  bool _isValid(int x, int y, Color prevC, Color newC);
  void _floodFill(int x, int y, Color prevC, Color newC);
};
}  // namespace CTRPluginFramework
