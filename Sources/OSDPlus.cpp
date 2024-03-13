#include "OSDPlus.hpp"

namespace CTRPluginFramework {
/* Utility functions for OSD */
// Thanks: https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm)
void DrawLine(const Screen &scr, int srcX, int srcY, int dstX, int dstY, const Color &color) {
  float x, y, dx, dy, step;
  int i;

  dx = (dstX - srcX);
  dy = (dstY - srcY);

  if (abs(dx) >= abs(dy)) {
    step = abs(dx);
  } else {
    step = abs(dy);
  }

  dx = dx / step;
  dy = dy / step;
  x = srcX;
  y = srcY;
  i = 1;

  while (i <= step) {
    scr.DrawPixel(x, y, color);
    x = x + dx;
    y = y + dy;
    i++;
  }
}

void DrawRectPlus(const Screen &scr, u32 posX, u32 posY, u32 width, u32 height, const Color &color, bool filled, int origin) {
  if (origin == 0) {
  } else if (origin == 1) {
    posX = posX - (width / 2);
  } else if (origin == 2) {
    posX = posX - width;
  } else if (origin == 3) {
    posX = posX - width;
    posY = posY - (height / 2);
  } else if (origin == 4) {
    posX = posX - width;
    posY = posY - height;
  } else if (origin == 5) {
    posX = posX - (width / 2);
    posY = posY - height;
  } else if (origin == 6) {
    posY = posY - height;
  } else if (origin == 7) {
    posY = posY - (height / 2);
  } else if (origin == 8) {
    posX = posX - (width / 2);
    posY = posY - (height / 2);
  }

  scr.DrawRect(posX, posY, width, height, color, filled);
}

void DrawPlus(const Screen &scr, const std::string &str, u32 posX, u32 posY, u32 borderWidth, u32 padding, const Color &foreground, const Color &background, const Color &border, int fontAlign, int origin) {
  int bgWidth = OSD::GetTextWidth(false, str);
  int height = 10 + padding * 2;

  if (origin == 0) {
  } else if (origin == 1) {
    posX = posX - ((borderWidth * 2 + padding * 2 + bgWidth) / 2);
  } else if (origin == 2) {
    posX = posX - (borderWidth * 2 + padding * 2 + bgWidth);
  } else if (origin == 3) {
    posX = posX - (borderWidth * 2 + padding * 2 + bgWidth);
    posY = posY - ((borderWidth * 2 + padding * 2 + 10) / 2);
  } else if (origin == 4) {
    posX = posX - (borderWidth * 2 + padding * 2 + bgWidth);
    posY = posY - (borderWidth * 2 + padding * 2 + 10);
  } else if (origin == 5) {
    posX = posX - ((borderWidth * 2 + padding * 2 + bgWidth) / 2);
    posY = posY - (borderWidth * 2 + padding * 2 + 10);
  } else if (origin == 6) {
    posY = posY - (borderWidth * 2 + padding * 2 + 10);
  } else if (origin == 7) {
    posY = posY - ((borderWidth * 2 + padding * 2 + 10) / 2);
  } else if (origin == 8) {
    posX = posX - ((borderWidth * 2 + padding * 2 + bgWidth) / 2);
    posY = posY - ((borderWidth * 2 + padding * 2 + 10) / 2);
  }

  scr.DrawRect(posX, posY, bgWidth + (borderWidth * 2) + (padding * 2), borderWidth, border);
  scr.DrawRect(posX + borderWidth + padding + bgWidth + padding, posY + borderWidth, borderWidth, height, border);
  scr.DrawRect(posX, posY + borderWidth + height, bgWidth + (borderWidth * 2) + (padding) * 2, borderWidth, border);
  scr.DrawRect(posX, posY + borderWidth, borderWidth, height, border);

  scr.DrawRect(
      posX + borderWidth,
      posY + borderWidth,
      bgWidth + padding * 2,
      10 + padding * 2,
      background);

  u32 strX = 0;
  u32 strY = 0;
  if (fontAlign == 0) {
    strX = posX + borderWidth;
    strY = posY + borderWidth;
  } else if (fontAlign == 1) {
    strX = posX + borderWidth + padding;
    strY = posY + borderWidth;
  } else if (fontAlign == 2) {
    strX = posX + borderWidth + (padding * 2);
    strY = posY + borderWidth;
  } else if (fontAlign == 3) {
    strX = posX + borderWidth + (padding * 2);
    strY = posY + borderWidth + padding;
  } else if (fontAlign == 4) {
    strX = posX + borderWidth + (padding * 2);
    strY = posY + borderWidth + padding * 2;
  } else if (fontAlign == 5) {
    strX = posX + borderWidth + padding;
    strY = posY + borderWidth + padding * 2;
  } else if (fontAlign == 6) {
    strX = posX + borderWidth;
    strY = posY + borderWidth + padding * 2;
  } else if (fontAlign == 7) {
    strX = posX + borderWidth;
    strY = posY + borderWidth + padding;
  } else if (fontAlign == 8) {
    strX = posX + borderWidth + padding;
    strY = posY + borderWidth + padding;
  }

  scr.Draw(
      str,
      strX,
      strY,
      foreground,
      background);
}

void DrawSysfontPlus(const Screen &scr, const std::string &str, u32 posX, u32 posY, u32 borderWidth, u32 padding, const Color &foreground, const Color &background, const Color &border, bool fillBackground, int fontAlign, int origin) {
  int bgWidth = OSD::GetTextWidth(true, str);
  int height = 16 + padding * 2;

  if (origin == 0) {
  } else if (origin == 1) {
    posX = posX - ((borderWidth * 2 + padding * 2 + bgWidth) / 2);
  } else if (origin == 2) {
    posX = posX - (borderWidth * 2 + padding * 2 + bgWidth);
  } else if (origin == 3) {
    posX = posX - (borderWidth * 2 + padding * 2 + bgWidth);
    posY = posY - ((borderWidth * 2 + padding * 2 + 16) / 2);
  } else if (origin == 4) {
    posX = posX - (borderWidth * 2 + padding * 2 + bgWidth);
    posY = posY - (borderWidth * 2 + padding * 2 + 16);
  } else if (origin == 5) {
    posX = posX - ((borderWidth * 2 + padding * 2 + bgWidth) / 2);
    posY = posY - (borderWidth * 2 + padding * 2 + 16);
  } else if (origin == 6) {
    posY = posY - (borderWidth * 2 + padding * 2 + 16);
  } else if (origin == 7) {
    posY = posY - ((borderWidth * 2 + padding * 2 + 16) / 2);
  } else if (origin == 8) {
    posX = posX - ((borderWidth * 2 + padding * 2 + bgWidth) / 2);
    posY = posY - ((borderWidth * 2 + padding * 2 + 16) / 2);
  }

  scr.DrawRect(posX, posY, bgWidth + (borderWidth * 2) + (padding * 2), borderWidth, border);
  scr.DrawRect(posX + borderWidth + padding + bgWidth + padding, posY + borderWidth, borderWidth, height, border);
  scr.DrawRect(posX, posY + borderWidth + height, bgWidth + (borderWidth * 2) + (padding) * 2, borderWidth, border);
  scr.DrawRect(posX, posY + borderWidth, borderWidth, height, border);

  if (fillBackground) {
    scr.DrawRect(posX + borderWidth, posY + borderWidth, bgWidth + padding * 2, 16 + padding * 2, background);
  }

  u32 strX = 0;
  u32 strY = 0;
  if (fontAlign == 0) {
    strX = posX + borderWidth;
    strY = posY + borderWidth;
  } else if (fontAlign == 1) {
    strX = posX + borderWidth + padding;
    strY = posY + borderWidth;
  } else if (fontAlign == 2) {
    strX = posX + borderWidth + (padding * 2);
    strY = posY + borderWidth;
  } else if (fontAlign == 3) {
    strX = posX + borderWidth + (padding * 2);
    strY = posY + borderWidth + padding;
  } else if (fontAlign == 4) {
    strX = posX + borderWidth + (padding * 2);
    strY = posY + borderWidth + padding * 2;
  } else if (fontAlign == 5) {
    strX = posX + borderWidth + padding;
    strY = posY + borderWidth + padding * 2;
  } else if (fontAlign == 6) {
    strX = posX + borderWidth;
    strY = posY + borderWidth + padding * 2;
  } else if (fontAlign == 7) {
    strX = posX + borderWidth;
    strY = posY + borderWidth + padding;
  } else if (fontAlign == 8) {
    strX = posX + borderWidth + padding;
    strY = posY + borderWidth + padding;
  }

  scr.DrawSysfont(str, posX + borderWidth + padding, posY + borderWidth + padding, foreground);
}

float DegreeToRadian(float degree) {
  return degree * (std::numbers::pi / 180);
}

void DrawCircle(const Screen &scr, u32 x, u32 y, u32 radiusStart, u32 radiusEnd, int start, int end, const Color &color, int origin) {
  u32 rectLength = (radiusEnd * 2) / 1.41421356237;
  u32 miniRadius = rectLength / 2;

  u32 rectX = x - miniRadius;
  u32 rectY = y - miniRadius;

  if (start == 0 && end == 360 && radiusStart == 0) {
    scr.DrawRect(rectX, rectY, rectLength, rectLength, color);
  } else {
    miniRadius = radiusStart;
  }

  if (origin == 0) {
    x = x + radiusEnd;
    y = y + radiusEnd;
  } else if (origin == 1) {
    y = y + radiusEnd;
  } else if (origin == 2) {
    x = x - radiusEnd;
    y = y + radiusEnd;
  } else if (origin == 3) {
    x = x - radiusEnd;
  } else if (origin == 4) {
    x = x - radiusEnd;
    y = y - radiusEnd;
  } else if (origin == 5) {
    y = y - radiusEnd;
  } else if (origin == 6) {
    x = x + radiusEnd;
    y = y - radiusEnd;
  } else if (origin == 7) {
    x = x + radiusEnd;
  } else if (origin == 8) {
  }

  for (int r = miniRadius; r < radiusEnd; r++) {
    for (int angle = start; angle < end; angle++) {
      scr.DrawPixel(x + cos(DegreeToRadian(angle)) * r, y + sin(DegreeToRadian(angle)) * r, color);
    }
  }
}
/* End of Utility functions for OSD */

void DrawDiamond(const Screen &scr, u16 leftX, u16 leftY, u16 rightX, u16 rightY, const Color &color, bool filled) {
  if (filled) {
    float x, y, dx, dy, step, xx, yy, dxx, dyy, step2;
    int i, j;

    dx = leftX - rightX;
    dy = leftY - rightY;

    step = abs(dx) + abs(dy);

    dx /= step;
    dy /= step;
    x = rightX;
    y = rightY;
    i = 1;

    dxx = rightX - leftX;
    dyy = leftY - rightY;

    step2 = abs(dxx) + abs(dyy);

    dxx /= step2;
    dyy /= step2;
    while (i <= step) {
      xx = x;
      yy = y;
      j = 1;
      while (j <= step2) {
        scr.DrawPixel(xx, yy, color);
        xx += dxx;
        yy += dyy;
        j++;
      }
      x += dx;
      y += dy;
      i++;
    }
  } else {
    DrawLine(scr, leftX, leftY, rightX, rightY, color);
    DrawLine(scr, rightX, rightY, rightX * 2 - leftX, leftY, color);
    DrawLine(scr, leftX, leftY, rightX, leftY * 2 - rightY, color);
    DrawLine(scr, rightX, leftY * 2 - rightY, rightX * 2 - leftX, leftY, color);
  }
}
}  // namespace CTRPluginFramework
