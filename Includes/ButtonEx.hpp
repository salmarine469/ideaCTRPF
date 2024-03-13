#pragma once

#include "KaniCodes.hpp"
#include "optional"

namespace CTRPluginFramework {
class ButtonEx {
 public:
  ButtonEx(const Screen &scr, const std::string &str, u32 posX, u32 posY, u32 padding = 10, const Color &foreground = Color::White, const Color &background = Color::Black, const Color &pressed = Color(10, 10, 10), u8 fontAlign = 8, u8 origin = 8);

  void Draw(void) const;
  void Update(void);

  bool IsDown(void) const;
  bool IsPressed(void) const;
  bool IsReleased(void) const;

 private:
  const Screen &m_scr;
  const std::string m_str;
  u32 m_posX, m_posY, m_padding, m_x, m_y, m_w, m_h;
  const Color m_foreground, m_background, m_pressed;
  bool m_fillBackground, m_isDown, m_isPressed, m_isReleased, m_isKeepPressed;
  u8 m_fontAlign, m_origin;
  std::optional<UIntVector> m_lastTouch;
};
}  // namespace CTRPluginFramework
