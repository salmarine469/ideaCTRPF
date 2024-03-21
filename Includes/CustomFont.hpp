#pragma once

#include <freetype/freetype.h>
#include <ft2build.h>

#include <CTRPluginFramework.hpp>
#include <algorithm>
#include <codecvt>
#include <locale>
#include <memory>

namespace CTRPluginFramework {
class CustomFont {
 public:
  CustomFont(const std::string &path, u32 size);

  void Draw(const std::string &str, const Screen &scr, u32 posX, u32 posY, const Color &foreground = Color::White, const Color &background = Color(0, 0, 0, 0)) const;
  unsigned int GetWidth(const std::string &str) const;
  unsigned int GetHeight(const std::string &str) const;
  explicit operator bool() const { return m_face != nullptr; }

 private:
  FT_Library m_library = nullptr;
  FT_Face m_face = nullptr;

  std::unique_ptr<u8[]> m_font;
};
}  // namespace CTRPluginFramework