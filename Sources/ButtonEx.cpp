#include "ButtonEx.hpp"

namespace CTRPluginFramework {
ButtonEx::ButtonEx(const Screen &scr, const std::string &str, u32 posX, u32 posY, u32 padding, const Color &foreground, const Color &background, const Color &pressed, u8 fontAlign, u8 origin) : m_scr(scr), m_str(str), m_posX(posX), m_posY(posY), m_padding(padding), m_foreground(foreground), m_background(background), m_pressed(pressed), m_fontAlign(fontAlign), m_origin(origin) {
  int bgWidth = OSD::GetTextWidth(true, str);
  if (origin == 0) {
  } else if (origin == 1) {
    posX = posX - (padding * 2 + bgWidth) / 2;
  } else if (origin == 2) {
    posX = posX - padding * 2 + bgWidth;
  } else if (origin == 3) {
    posX = posX - padding * 2 + bgWidth;
    posY = posY - (padding * 2 + 16) / 2;
  } else if (origin == 4) {
    posX = posX - padding * 2 + bgWidth;
    posY = posY - padding * 2 + 16;
  } else if (origin == 5) {
    posX = posX - (padding * 2 + bgWidth) / 2;
    posY = posY - padding * 2 + 16;
  } else if (origin == 6) {
    posY = posY - padding * 2 + 16;
  } else if (origin == 7) {
    posY = posY - (padding * 2 + 16) / 2;
  } else if (origin == 8) {
    posX = posX - (padding * 2 + bgWidth) / 2;
    posY = posY - (padding * 2 + 16) / 2;
  }

  m_x = posX;
  m_y = posY;
  m_w = bgWidth + (padding * 2);
  m_h = 16 + padding * 2;
}

void ButtonEx::Draw(void) const {
  auto background = m_background;
  if (m_isPressed)
    background = m_background.Blend(m_pressed, Color::BlendMode::Alpha);
  DrawSysfontPlus(m_scr, m_str, m_posX, m_posY, 0, m_padding, m_foreground, background, Color::Black, true, m_fontAlign, m_origin);
}

void ButtonEx::Update(void) {
  bool touch = TouchRect(m_x, m_y, m_w, m_h);
  if (touch && !m_lastTouch) {
    m_isDown = true;
    m_isKeepPressed = true;
  } else
    m_isDown = false;

  if (!Touch::IsDown() && m_lastTouch && TouchRect(m_x, m_y, m_w, m_h, *m_lastTouch) && m_isKeepPressed)
    m_isReleased = true;
  else
    m_isReleased = false;

  if (!touch)
    m_isKeepPressed = false;

  m_isPressed = touch;

  if (Touch::IsDown())
    m_lastTouch = Touch::GetPosition();
  else
    m_lastTouch = std::nullopt;
}

bool ButtonEx::IsDown(void) const {
  return m_isDown;
}

bool ButtonEx::IsPressed(void) const {
  return m_isPressed;
}

bool ButtonEx::IsReleased(void) const {
  return m_isReleased;
}

}  // namespace CTRPluginFramework
