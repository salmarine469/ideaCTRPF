#pragma once

#include <freetype/freetype.h>
#include <ft2build.h>

#include <CTRPluginFramework.hpp>
#include <codecvt>
#include <algorithm>
#include <locale>
#include <memory>

namespace CTRPluginFramework {
class CustomFont {
 public:
  CustomFont(const std::string &path, u32 size);

  void Draw(const std::string &str, const Screen &scr, u32 posX, u32 posY, const Color &foreground = Color::White, const Color &background = Color::Black) const;

 private:
  FT_Library m_library;
  FT_Face m_face;

  std::unique_ptr<u8[]> m_font;
};
}  // namespace CTRPluginFramework