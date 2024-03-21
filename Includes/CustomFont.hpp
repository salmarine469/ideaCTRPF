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

  void Draw(const std::string &str, const Screen &scr, u32 posX, u32 posY, const Color &foreground = Color::White, const Color &background = Color(0, 0, 0, 0));
  unsigned int GetWidth(const std::string &str) const;
  unsigned int GetHeight(const std::string &str) const;
  explicit operator bool() const { return m_face != nullptr; }
  bool HasCached(void) const { return m_hasCache; }
  void DrawCached(const Screen &scr, u32 posX, u32 posY) const;

 private:
  FT_Library m_library = nullptr;
  FT_Face m_face = nullptr;

  std::unique_ptr<u8[]> m_font;

  bool m_hasCache = false;
  std::vector<std::pair<UIntVector, Color>> m_cache;
};
}  // namespace CTRPluginFramework