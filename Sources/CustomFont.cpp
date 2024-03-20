#include "CustomFont.hpp"

namespace CTRPluginFramework {
CustomFont::CustomFont(const std::string &path, u32 size) {
  FT_Init_FreeType(&m_library);
  File fontFile(path, File::READ);
  if (!fontFile.IsOpen()) {
    MessageBox("Failed to open font file", DialogType::DialogOk);
    return;
  }

  m_font = std::make_unique<u8[]>(fontFile.GetSize());
  if (!m_font) {
    MessageBox("Failed to allocate memory for font", DialogType::DialogOk);
    return;
  }

  fontFile.Read(m_font.get(), fontFile.GetSize());

  FT_New_Memory_Face(m_library, m_font.get(), fontFile.GetSize(), 0, &m_face);
  FT_Set_Pixel_Sizes(m_face, 0, size);
}

void CustomFont::Draw(const std::string &str, const Screen &scr, u32 posX, u32 posY, const Color &foreground, const Color &background) const {
  u32 x = posX;
  u32 y = posY;

  std::u32string utf32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(str);

  unsigned int last_height = 0;

  for (auto c : utf32) {
    auto error = FT_Load_Char(m_face, c, FT_LOAD_RENDER);
    if (error)
      continue;

    auto bitmap = m_face->glyph->bitmap;
    auto width = bitmap.width;
    auto height = bitmap.rows;

    if (c == '\n') {
      x = posX;
      y += last_height;
      continue;
    }

    last_height = std::max(last_height, height);

    if (background.a != 0)
      scr.DrawRect(x, y, width, height, background);

    for (u32 i = 0; i < width; i++) {
      for (u32 j = 0; j < height; j++) {
        auto color = bitmap.buffer[i + j * width];
        if (!color)
          continue;
        scr.DrawPixel(x + i, y + j, foreground);
      }
    }

    x += width;
  }
}
}  // namespace CTRPluginFramework
