#include "JPKeyboard.hpp"
#include "Convert.hpp"
#include "KaniCodes.hpp"

namespace CTRPluginFramework
{
  JPKeyboard::JPKeyboard(std::string text)
  {
    _text = text;
    _maxLength = 16;
    _canSwich = true;
    _canAbort = true;
    _canConvert = true;
    _flick = false;
    selectedIndex = 0;
    cursorPos = 0;
  }

  // https://gist.github.com/HidegonSan/e04334ed8d22691a9ac394a74ca1877f
  char *_u8slice(char *buf, const char *str, u32 begin, u32 end)
  {
    u32 count = 0;
    char *p = buf; // returnするポインタを格納
    while (*str != '\0')
    {
      if ((*str & 0xC0) != 0x80)
      {
        count++;
      }
      if (count > begin && count <= end)
      {
        *buf = *str;
        buf++;
      } // 指定範囲の文字を格納
      str++;
    }
    *buf = '\0'; // 末尾にNULL文字を格納
    return p;
  }

  // 正しく文字を切り出す
  std::string _string_subtract(const std::string &str, u32 start, u32 length)
  {
    char buf[128];
    char *sliced = _u8slice(buf, str.c_str(), start, start + length);
    std::string ret = sliced;
    return ret;
  }

  struct ConvertTable
  {
    std::initializer_list<char const *> cv_froms;
    char const *cv_target;
  };

  // Thanks: https://support.microsoft.com/ja-jp/topic/%E3%83%AD%E3%83%BC%E3%83%9E%E5%AD%97%E5%85%A5%E5%8A%9B%E3%81%AE%E3%81%A4%E3%81%A5%E3%82%8A%E4%B8%80%E8%A6%A7%E8%A1%A8%E3%82%92%E7%A2%BA%E8%AA%8D%E3%81%97%E3%81%A6%E3%81%BF%E3%82%88%E3%81%86-bcc0ad7e-2781-cc9a-e524-7de506d8fdae
  static const ConvertTable conv_table_list[] = {
      {{""}, ""},
      {{"a"}, "あ"},
      {{"i", "yi"}, "い"},
      {{"u", "wu", "whu"}, "う"},
      {{"e"}, "え"},
      {{"o"}, "お"},
      {{"wha"}, "うぁ"},
      {{"whi", "wi"}, "うぃ"},
      {{"whe", "we"}, "うぇ"},
      {{"who"}, "うぉ"},
      {{"la", "xa"}, "ぁ"},
      {{"li", "xi", "lyi", "xyi"}, "ぃ"},
      {{"lu", "xu"}, "ぅ"},
      {{"le", "xe", "lye", "xye"}, "ぇ"},
      {{"lo", "xo"}, "ぉ"},
      {{"ye"}, "いぇ"},
      {{"ka", "ca"}, "か"},
      {{"ki"}, "き"},
      {{"ku", "cu", "qu"}, "く"},
      {{"ke"}, "け"},
      {{"ko", "co"}, "こ"},
      {{"kya"}, "きゃ"},
      {{"kyi"}, "きぃ"},
      {{"kyu"}, "きゅ"},
      {{"kye"}, "きぇ"},
      {{"kyo"}, "きょ"},
      {{"qya"}, "くゃ"},
      {{"qyu"}, "くゅ"},
      {{"qyo"}, "くょ"},
      {{"qwa", "wa", "kwa"}, "くぁ"},
      {{"qui", "qi", "qyi"}, "くぃ"},
      {{"qwu"}, "くぅ"},
      {{"qwe", "qe", "qye"}, "くぇ"},
      {{"qwo", "qo"}, "くぉ"},
      {{"ga"}, "が"},
      {{"gi"}, "ぎ"},
      {{"gu"}, "ぐ"},
      {{"ge"}, "げ"},
      {{"go"}, "ご"},
      {{"gya"}, "ぎゃ"},
      {{"gyi"}, "ぎぃ"},
      {{"gyu"}, "ぎゅ"},
      {{"gye"}, "ぎぇ"},
      {{"gyo"}, "ぎょ"},
      {{"gwa"}, "ぐぁ"},
      {{"gwi"}, "ぐぃ"},
      {{"gwu"}, "ぐぅ"},
      {{"gwe"}, "ぐぇ"},
      {{"gwo"}, "ぐぉ"},
      {{"lka", "xka"}, "ヵ"},
      {{"lke", "xke"}, "ヶ"},
      {{"sa"}, "さ"},
      {{"si", "ci", "shi"}, "し"},
      {{"su"}, "す"},
      {{"se", "ce"}, "せ"},
      {{"so"}, "そ"},
      {{"sya", "sha"}, "しゃ"},
      {{"syi"}, "しぃ"},
      {{"syu", "shu"}, "しゅ"},
      {{"sye", "she"}, "しぇ"},
      {{"syo", "sho"}, "しょ"},
      {{"swa"}, "すぁ"},
      {{"swi"}, "すぃ"},
      {{"swu"}, "すぅ"},
      {{"swe"}, "すぇ"},
      {{"swo"}, "すぉ"},
      {{"za"}, "ざ"},
      {{"zi", "ji"}, "じ"},
      {{"zu"}, "ず"},
      {{"ze"}, "ぜ"},
      {{"zo"}, "ぞ"},
      {{"zya", "ja", "jya"}, "じゃ"},
      {{"zyi", "jyi"}, "じぃ"},
      {{"zyu", "ju", "jyu"}, "じゅ"},
      {{"zye", "je", "jye"}, "じぇ"},
      {{"zyo", "jo", "jyo"}, "じょ"},
      {{"ta"}, "た"},
      {{"ti", "chi"}, "ち"},
      {{"tu", "tsu"}, "つ"},
      {{"te"}, "て"},
      {{"to"}, "と"},
      {{"tya", "cha", "cya"}, "ちゃ"},
      {{"tyi", "cyi"}, "ちぃ"},
      {{"tyu", "chu", "cyu"}, "ちゅ"},
      {{"tye", "che", "cye"}, "ちぇ"},
      {{"tyo", "cho", "cyo"}, "ちょ"},
      {{"tsa"}, "つぁ"},
      {{"tsi"}, "つぃ"},
      {{"tse"}, "つぇ"},
      {{"tso"}, "つぉ"},
      {{"tha"}, "てゃ"},
      {{"thi"}, "てぃ"},
      {{"thu"}, "てゅ"},
      {{"the"}, "てぇ"},
      {{"tho"}, "てょ"},
      {{"twa"}, "とぁ"},
      {{"twi"}, "とぃ"},
      {{"twu"}, "とぅ"},
      {{"twe"}, "とぇ"},
      {{"two"}, "とぉ"},
      {{"da"}, "だ"},
      {{"di"}, "ぢ"},
      {{"du"}, "づ"},
      {{"de"}, "で"},
      {{"do"}, "ど"},
      {{"dya"}, "ぢゃ"},
      {{"dyi"}, "ぢぃ"},
      {{"dyu"}, "ぢゅ"},
      {{"dye"}, "ぢぇ"},
      {{"dyo"}, "ぢょ"},
      {{"dha"}, "でゃ"},
      {{"dhi"}, "でぃ"},
      {{"dhu"}, "でゅ"},
      {{"dhe"}, "でぇ"},
      {{"dho"}, "でょ"},
      {{"dwa"}, "どぁ"},
      {{"dwi"}, "どぃ"},
      {{"dwu"}, "どぅ"},
      {{"dwe"}, "どぇ"},
      {{"dwo"}, "どぉ"},
      {{"ltu", "xtu", "ltsu"}, "っ"},
      {{"na"}, "な"},
      {{"ni"}, "に"},
      {{"nu"}, "ぬ"},
      {{"ne"}, "ね"},
      {{"no"}, "の"},
      {{"nya"}, "にゃ"},
      {{"nyi"}, "にぃ"},
      {{"nyu"}, "にゅ"},
      {{"nye"}, "にぇ"},
      {{"nyo"}, "にょ"},
      {{"ha"}, "は"},
      {{"hi"}, "ひ"},
      {{"hu", "fu"}, "ふ"},
      {{"he"}, "へ"},
      {{"ho"}, "ほ"},
      {{"hya"}, "ひゃ"},
      {{"hyi"}, "ひぃ"},
      {{"hyu"}, "ひゅ"},
      {{"hye"}, "ひぇ"},
      {{"hyo"}, "ひょ"},
      {{"fwa", "fa"}, "ふぁ"},
      {{"fwi", "fi", "fyi"}, "ふぃ"},
      {{"fwu"}, "ふぅ"},
      {{"fwe", "fe", "fye"}, "ふぇ"},
      {{"fwo", "fo"}, "ふぉ"},
      {{"fya"}, "ふゃ"},
      {{"fyu"}, "ふゅ"},
      {{"fyo"}, "ふょ"},
      {{"ba"}, "ば"},
      {{"bi"}, "び"},
      {{"bu"}, "ぶ"},
      {{"be"}, "べ"},
      {{"bo"}, "ぼ"},
      {{"bya"}, "びゃ"},
      {{"byi"}, "びぃ"},
      {{"byu"}, "びゅ"},
      {{"bye"}, "びぇ"},
      {{"byo"}, "びょ"},
      {{"va"}, "ゔぁ"},
      {{"vi", "vyi"}, "ゔぃ"},
      {{"vu"}, "ゔ"},
      {{"ve", "vye"}, "ゔぇ"},
      {{"vo"}, "ゔぉ"},
      {{"vya"}, "ゔゃ"},
      {{"vyu"}, "ゔゅ"},
      {{"vyo"}, "ゔょ"},
      {{"pa"}, "ぱ"},
      {{"pi"}, "ぴ"},
      {{"pu"}, "ぷ"},
      {{"pe"}, "ぺ"},
      {{"po"}, "ぽ"},
      {{"pya"}, "ぴゃ"},
      {{"pyi"}, "ぴぃ"},
      {{"pyu"}, "ぴゅ"},
      {{"pye"}, "ぴぇ"},
      {{"pyo"}, "ぴょ"},
      {{"ma"}, "ま"},
      {{"mi"}, "み"},
      {{"mu"}, "む"},
      {{"me"}, "め"},
      {{"mo"}, "も"},
      {{"mya"}, "みゃ"},
      {{"myi"}, "みぃ"},
      {{"myu"}, "みゅ"},
      {{"mye"}, "みぇ"},
      {{"myo"}, "みょ"},
      {{"ya"}, "や"},
      {{"yu"}, "ゆ"},
      {{"yo"}, "よ"},
      {{"lya", "xya"}, "ゃ"},
      {{"lyu", "xyu"}, "ゅ"},
      {{"lyo", "xyo"}, "ょ"},
      {{"ra"}, "ら"},
      {{"ri"}, "り"},
      {{"ru"}, "る"},
      {{"re"}, "れ"},
      {{"ro"}, "ろ"},
      {{"rya"}, "りゃ"},
      {{"ryi"}, "りぃ"},
      {{"ryu"}, "りゅ"},
      {{"rye"}, "りぇ"},
      {{"ryo"}, "りょ"},
      {{"wa"}, "わ"},
      {{"wo"}, "を"},
      {{"n", "nn", "xn"}, "ん"},
      {{"lwa", "xwa"}, "ゎ"},
  };

  ConvertTable const &find_convert_table(std::string const &_val)
  {
    for (auto &&item : conv_table_list)
      for (auto &&s : item.cv_froms)
      {
        if (strcmp(s, _val.c_str()) == 0)
          return item;
      }

    return conv_table_list[0];
  }

  std::string _alphabet_to_hiragana(std::string input)
  {
    if (input.empty() || 5 < input.size())
      return "";

    transform(input.begin(), input.end(), input.begin(), tolower);
    std::string ret = "";

    if (2 <= input.size())
    { // 同じ文字が続けられていたら "っ" を追加する
      if (input[0] == input[1] && (input[0] != 'n'))
      {
        ret += "っ";
        input = _string_subtract(input, 1, input.size() - 1);
      }
    }

    auto &item = find_convert_table(input);
    if (*item.cv_target != '\0' && item.cv_target != nullptr)
    {
      return ret + std::string{item.cv_target};
    }

    if (2 <= input.size() && input[0] == 'n')
    { // N の後に文字が続いていてかつその後に続いている文字が次の結果に影響しない (nu, nyaなどはダメ) 場合、 "ん" に変換する
      char ng_chars[] = {'a', 'i', 'u', 'e', 'o', 'y'};
      for (auto &&ng_char : ng_chars)
      {
        if (input[1] != ng_char && ng_char == 'y')
        {
          return "ん" + _string_subtract(input, 1, input.size() - 1);
        }
      }
    }

    return ret + input;
  }

  std::string jpQwertyOutput = "";
  void JPKeyboardEvent(Keyboard &keyboard, KeyboardEvent &event)
  {
    if (event.type == event.CharacterAdded || event.type == event.CharacterRemoved)
    {
      jpQwertyOutput = "";
      std::string str = keyboard.GetInput();
      while (1)
      {
        size_t len = 1;
        while (1)
        {
          if (len <= str.length())
          {
            if (len == 1 && 2 <= str.length() && str[0] == 'n' && std::string("aiueoy").find(str[1] != std::string::npos))
              len++;
            if (_alphabet_to_hiragana(str.substr(0, len)).back() == str.substr(0, len).back() && len < str.length())
            {
              if (len < 3)
                len++;
              else
              {
                len = 1;
                if (0 < str.length())
                {
                  jpQwertyOutput += str.substr(0, 1);
                  str = str.substr(1);
                }
                else
                  break;
              }
              continue;
            }
          }
          break;
        }
        if (len <= str.length())
        {
          jpQwertyOutput += _alphabet_to_hiragana(str.substr(0, len));
          str = str.substr(len);
        }
        else
          break;
      }
    }
    size_t i = 0;
    while (true)
    {
      if (OSD::GetTextWidth(true, jpQwertyOutput.substr(i, jpQwertyOutput.length() - i)) <= 270)
        break;
      i += 3;
    }

    OSD::GetBottomScreen().DrawRect(20, 20, 280, 25, Color::Black);
    OSD::GetBottomScreen().DrawSysfont(jpQwertyOutput.substr(i, jpQwertyOutput.length() - i), 23, 23);
  }

  bool JPKeyboard::LoadKanjiList(void)
  {
    u32 count = 0;
    if (File::Exists("kanji.txt"))
    {
      File file("kanji.txt");
      size_t size = file.GetSize();
      char buf[size + 1] = {0};
      file.Read(buf, size);
      std::string str{buf, std::min(size, str.max_size())};
      while (str.find(',') != std::string::npos)
      {
        while (str.find('\n') < str.find(','))
          str = str.substr(str.find('\n') != std::string::npos ? str.find('\n') + 1 : str.length());
        std::string hiragana = str.substr(0, str.find(',') != std::string::npos ? str.find(',') : str.length()), kanji;
        str = str.substr(str.find(',') != std::string::npos ? str.find(',') + 1 : str.length());
        kanji = str.substr(0, str.find('\n') != std::string::npos ? str.find('\n') : str.length());
        str = str.substr(str.find('\n') != std::string::npos ? str.find('\n') + 1 : str.length());
        Convert::addHiraganaKanjiList(hiragana, kanji);
      }
      file.Close();
      OSD::Notify(Color::Green << "kanji.txt loaded" << Color::White);
      return true;
    }
    else
    {
      OSD::Notify("kanji.txt not found.");
      return false;
    }
    return false;
  }

  JPKeyboard JPKeyboard::SetMaxLength(u32 max)
  {
    _maxLength = max;
    return *this;
  }

  JPKeyboard JPKeyboard::CanSwichLayout(bool canSwich)
  {
    _canSwich = canSwich;
    return *this;
  }
  JPKeyboard JPKeyboard::CanAbort(bool canAbort)
  {
    _canAbort = canAbort;
    return *this;
  }
  JPKeyboard JPKeyboard::CanConvert(bool canConvert)
  {
    _canConvert = canConvert;
    return *this;
  }
  JPKeyboard JPKeyboard::SetDefaultLayout(bool isFlick)
  {
    _flick = isFlick;
    return *this;
  }

  void JPKeyboard::MakeU16Array()
  {
    Process::WriteString((u32)U16_ChrArray, _flick ? (KatakanaMode ? FlickKatakana : FlickHiragana) : (KatakanaMode ? Katakana : Hiragana), StringFormat::Utf16);
  }

  void JPKeyboard::Komoji(u16 &moji)
  {
    const std::string str1 = "あいうえおアイウエオわワカつツやゆよヤユヨケ";
    const std::string str2 = "ぁぃぅぇぉァィゥェォゎヮヵっッゃゅょャュョヶ";

    // UTF16文字配列
    u16 u16array1[100] = {0}; // 変換前
    u16 u16array2[100] = {0}; // 変換後

    Process::WriteString((u32)u16array1, str1, StringFormat::Utf16);
    Process::WriteString((u32)u16array2, str2, StringFormat::Utf16);
    for (int i = 0; i < 23; i++)
    {
      if (moji == u16array1[i])
        moji = u16array2[i];
      else if (moji == u16array2[i])
        moji = u16array1[i];
    }
  }

  void JPKeyboard::Dakuten(bool handakuten, u16 &moji)
  {

    // 濁点用
    const std::string str1 = "かきくけこさしすせそたちつてとはひふへほカキクケコサシスセソタチツテトハヒフヘホウゝヽ";
    const std::string str2 = "がぎぐげござじずぜぞだぢづでどばびぶべぼガギグゲゴザジズゼゾダヂヅデドバビブベボヴゞヾ";

    // 半濁点用
    const std::string str3 = "はひふへほハヒフヘホウ";
    const std::string str4 = "ぱぴぷぺぽパピプペポヴ";

    // UTF16文字配列
    u16 u16array1[100] = {0}; // 変換前
    u16 u16array2[100] = {0}; // 変換後

    if (handakuten) // 半濁点モード
    {
      Process::WriteString((u32)u16array1, str3, StringFormat::Utf16);
      Process::WriteString((u32)u16array2, str4, StringFormat::Utf16);
    }
    else // 濁点モード
    {
      Process::WriteString((u32)u16array1, str1, StringFormat::Utf16);
      Process::WriteString((u32)u16array2, str2, StringFormat::Utf16);
    }

    // 変換
    for (int i = 0; i < (handakuten ? 10 : 50); i++)
    {
      // ヒットしたら書き換えて終了
      if (u16array1[i] == moji)
        moji = u16array2[i];
      else if (u16array2[i] == moji)
        moji = u16array1[i];
    }
  }

  bool JPKeyboard::DrawKeyboard(const Screen &scr)
  {
    MakeU16Array();

    // 背景
    scr.DrawRect(20, 20, 280, 200, Color::Black);
    scr.DrawRect(22, 22, 276, 196, Color::White, false);

    // キー描画
    std::string key;
    if (_flick)
      for (u8 x = 0; x < 3; x++)
        for (u8 y = 0; y < 4; y++)
        {
          u16 sx = 120 + x * 30;
          u16 sy = 70 + y * 30;
          key.clear();
          Process::ReadString((u32)&U16_ChrArray[(y * 3 + x) * 5], key, 2, StringFormat::Utf16);
          scr.DrawSysfont(key, sx + 5, sy + 3, Color::White);
        }
    else
      for (u8 x = 0; x < 10; x++)
        for (u8 y = 0; y < 5; y++)
        {
          u32 sx = 23 + x * 24;
          u32 sy = 68 + y * 22;

          // 文字
          key.clear();
          Process::ReadString((u32)&U16_ChrArray[y * 10 + x], key, 2, StringFormat::Utf16);
          scr.DrawSysfont(key, sx + 5, sy + 3, Color::White);
        }

    // 入力中の文字列描画
    scr.DrawRect(55, 56, 212, 1, Color::White);

    u16 str[100] = {0};

    InputStr.clear();
    std::memcpy(str, InputChrs.data(), sizeof(u16) * InputChrs.size());
    Process::ReadString((u32)str, InputStr, sizeof(u16) * InputChrs.size(), StringFormat::Utf16);

    textBegin = 0;
    textLen = 1;
    textWidth = OSD::GetTextWidth(true, InputStr);
    if (208 < textWidth)
    {
      for (; textBegin < InputChrs.size(); textBegin++)
      {
        InputStr.clear();
        Process::ReadString((u32)&str[textBegin], InputStr, sizeof(u16) * (InputChrs.size() - textBegin), StringFormat::Utf16);
        textWidth = OSD::GetTextWidth(true, InputStr);
        if (textWidth <= 208)
          break;
      }
      textBegin = std::min(0 < InputChrs.size() - cursorPos - (0 < selectedIndex ? selectedIndex : 0) ? InputChrs.size() - cursorPos - (0 < selectedIndex ? selectedIndex : 0) - 1 : InputChrs.size() - cursorPos - (0 < selectedIndex ? selectedIndex : 0), size_t(textBegin));
    }
    for (; textLen <= InputChrs.size() - textBegin; textLen++)
    {
      InputStr.clear();
      Process::ReadString((u32)&str[textBegin], InputStr, sizeof(u16) * textLen, StringFormat::Utf16);
      textWidth = OSD::GetTextWidth(true, InputStr);
      if (208 < textWidth)
        break;
    }
    InputStr.clear();
    Process::ReadString((u32)&str[textBegin], InputStr, sizeof(u16) * --textLen, StringFormat::Utf16);
    textWidth = OSD::GetTextWidth(true, InputStr);

    std::string selectedString;
    if (!InputChrs.empty())
    {
      std::memset(str, 0, sizeof(u16) * 100);
      if (selectedIndex < 0)
        std::memcpy(str, &InputChrs[InputChrs.size() - cursorPos], sizeof(u16) * abs(selectedIndex));
      else
        std::memcpy(str, &InputChrs[InputChrs.size() - cursorPos - selectedIndex], sizeof(u16) * selectedIndex);
      Utils::ConvertUTF16ToUTF8(selectedString, str);

      std::string cursorString;
      std::memset(str, 0, sizeof(u16) * 100);
      std::memcpy(str, &InputChrs[InputChrs.size() - cursorPos], sizeof(u16) * (cursorPos + textBegin + textLen - InputChrs.size()));
      Utils::ConvertUTF16ToUTF8(cursorString, str);

      if (selectedIndex < 0)
        scr.DrawRect(54 + textWidth - OSD::GetTextWidth(true, cursorString), 37, OSD::GetTextWidth(true, selectedString), 17, Color::Blue);
      else
        scr.DrawRect(54 + textWidth - OSD::GetTextWidth(true, cursorString) - OSD::GetTextWidth(true, selectedString), 37, OSD::GetTextWidth(true, selectedString), 17, Color::Blue);
      scr.DrawSysfont(InputStr, 54, 38);
      scr.DrawRect(54 + textWidth - OSD::GetTextWidth(true, cursorString), 37, 1, 17, Color::White);
    }
    else
      scr.DrawRect(54, 37, 1, 17, Color::White);

    // 右側のオプション
    const std::string opt[5] = {"削除", "小字", "ー", "”", "゜"};
    for (int i = 0; i < 5; i++)
    {
      scr.DrawSysfont(opt[i], 268, 72 + i * 22);
    }

    scr.DrawSysfont("<", 35, 35);
    scr.DrawSysfont(">", 277, 35);

    // とじる
    scr.DrawRect(28, 191, 68, 22, _canAbort ? Color::Gray : Color::BlackGrey);
    scr.DrawRect(28, 191, 68, 22, Color::White, false);
    scr.DrawSysfont("とじる", 42, 194);

    // モード変換
    scr.DrawRect(126, 191, 68, 22, Color::Gray);
    scr.DrawRect(126, 191, 68, 22, Color::White, false);
    scr.DrawSysfont("レイアウト", 128, 194);

    // 決定
    scr.DrawRect(224, 191, 68, 22, Color::Gray);
    scr.DrawRect(224, 191, 68, 22, Color::White, false);
    scr.DrawSysfont("決定", 246, 194);

    return true;
  }

  bool JPKeyboard::InputKeyboard(const Screen &scr, std::string &out)
  {
    // 右側のオプション
    const std::string opt[5] = {"削除", "小字", "ー", "”", "゜"};
    for (int i = 0; i < 5; i++)
    {
      if (Controller::IsKeyPressed(Touchpad) && TouchRect(268, 72 + i * 22, 24, 16))
      {
        switch (i)
        {
        case DELETE:
          if (!InputChrs.empty())
          {
            for (int j = 0; j < std::max(1, abs(selectedIndex)); j++)
              if (0 <= selectedIndex && InputChrs.size() != cursorPos)
                InputChrs.erase(InputChrs.end() - cursorPos - 1);
              else
                InputChrs.erase(InputChrs.end() - cursorPos--);
          }
          break;
        case KOMOJI:
          if (!InputChrs.empty())
            Komoji(InputChrs[InputChrs.size() - cursorPos - 1]);
          break;
        case HYPHEN:
          if (InputChrs.size() < _maxLength)
            InputChrs.insert(InputChrs.end() - cursorPos, 0x30FC);
          break;
        case DAKUTEN:
          if (!InputChrs.empty())
            Dakuten(false, InputChrs[InputChrs.size() - cursorPos - 1]);
          break;
        case HANDAKUTEN:
          if (!InputChrs.empty())
            Dakuten(true, InputChrs[InputChrs.size() - cursorPos - 1]);
          break;
        }
        selectedIndex = 0;
        scr.DrawRect(263, 68 + i * 22, 34, 22, Color::White);
      }
    }

    // 選択
    if (_canConvert)
    {
      if (((Controller::IsKeyPressed(Touchpad) && TouchRect(32, 32, 24, 22)) || Controller::IsKeyPressed(Key::L)) && selectedIndex < textLen - (textBegin ? 1 : 0))
      {
        scr.DrawRect(32, 35, 17, 17, Color::White);
        if (selectedIndex < s8(InputChrs.size() - cursorPos) || selectedIndex < 0)
          selectedIndex++;
      }
      if (((Controller::IsKeyPressed(Touchpad) && TouchRect(274, 32, 24, 22)) || Controller::IsKeyPressed(Key::R)) && (abs(selectedIndex) < textLen - (textBegin ? 1 : 0) || 0 < selectedIndex))
      {
        scr.DrawRect(274, 35, 17, 17, Color::White);
        if (abs(selectedIndex) < cursorPos || 0 < selectedIndex)
          selectedIndex--;
      }

      // 漢字変換
      if (Controller::IsKeyPressed(Key::Y) && selectedIndex != 0)
      {
        std::string hiragana;
        u16 buff[sizeof(u16) * abs(selectedIndex)] = {0};
        if (selectedIndex < 0)
          std::memcpy(buff, &InputChrs[InputChrs.size() - cursorPos], sizeof(u16) * abs(selectedIndex));
        else
          std::memcpy(buff, &InputChrs[InputChrs.size() - cursorPos - selectedIndex], sizeof(u16) * selectedIndex);
        Utils::ConvertUTF16ToUTF8(hiragana, buff);

        std::string kanji = Convert::hiraganaToKanji(hiragana);

        if (!kanji.empty())
        {
          if (Convert::getMultiByte(kanji) != -1)
          {
            for (int j = 0; j < abs(selectedIndex); j++)
              if (0 <= selectedIndex)
                InputChrs.erase(InputChrs.end() - cursorPos - 1);
              else
                InputChrs.erase(InputChrs.end() - cursorPos--);
            selectedIndex = 0;
            u16 buff_utf16[Convert::getMultiByte(kanji) + 1] = {0};
            Process::WriteString((u32)buff_utf16, kanji, StringFormat::Utf16);
            s8 i = -1;
            while (buff_utf16[++i] != 0 && buff_utf16[i] != 0xd && InputChrs.size() < _maxLength)
              InputChrs.insert(InputChrs.end() - cursorPos, buff_utf16[i]);
          }
        }
      }
    }

    // カーソル
    if (Controller::IsKeyPressed(Key::Left) && cursorPos < InputChrs.size())
    {
      cursorPos++;
      selectedIndex = 0;
    }
    if (Controller::IsKeyPressed(Key::Right) && cursorPos > 0)
    {
      cursorPos--;
      selectedIndex = 0;
    }
    if (TouchRect(54, 32, textWidth + 5, 27) && textBegin == 0)
    {
      UIntVector pos = Touch::GetPosition();
      u8 ii = 0;
      cursorPos = 0;
      for (auto &&InputChr : InputChrs)
      {
        if (u16(48 + textWidth) < pos.x)
          break;

        if (InputChr < 0x1000)
          ii++;
        else
          ii += 3;

        textWidth = OSD::GetTextWidth(true, InputStr.substr(ii, InputStr.length() - ii));
        cursorPos++;
      }
      selectedIndex = 0;
    }

    // とじる
    if ((Controller::IsKeyPressed(B) || TouchRect(28, 191, 68, 22)) && _canAbort)
    {
      KeyboardOpened = false;
      return false;
    }

    // モード変換
    if (Controller::IsKeyPressed(Touchpad) && TouchRect(126, 191, 68, 22))
    {
      if (KatakanaMode || !_canSwich)
      {
        std::string input;
        if (0 <= Keyboard("ローマ字").Open(input))
          for (size_t i = 0; i < input.length(); i++)
          {
            if (_maxLength <= InputChrs.size())
              break;
            u16 buff;
            Process::WriteString((u32)&buff, input.substr(i, 1), StringFormat::Utf16);
            InputChrs.insert(InputChrs.end() - cursorPos, buff);
            selectedIndex = 0;
          }
        if (_canSwich)
        {
          jpQwertyOutput = "";
          Keyboard key("日本語");
          key.OnKeyboardEvent(JPKeyboardEvent);
          if (0 <= key.Open(input))
          {
            memset(U16_ChrArray, 0, sizeof(U16_ChrArray));
            Process::WriteString((u32)U16_ChrArray, jpQwertyOutput.substr(0, std::min(size_t(_maxLength) * 3, size_t(180))), StringFormat::Utf16);
            size_t i = 0;
            while (U16_ChrArray[i] != 0 && InputChrs.size() < _maxLength)
              InputChrs.insert(InputChrs.end() - cursorPos, U16_ChrArray[i++]);
            selectedIndex = 0;
          }
        }
        if (_canSwich)
          _flick = !_flick;
      }
      if (_canSwich)
        KatakanaMode = !KatakanaMode;
    }

    // 決定
    if (TouchRect(224, 191, 68, 22))
    {
      if (!InputChrs.empty())
      {
        out = InputStr;
        KeyboardOpened = false;
        return true;
      }
    }

    // 入力
    if (Controller::IsKeyPressed(Touchpad))
    {
      UIntVector pos = Touch::GetPosition();
      if (_flick)
      {
        if (pos.x >= 120 && pos.y >= 70 && pos.x <= 200 && pos.y <= 190)
        {
          int wx = pos.x - 121;
          int wy = pos.y - 71;
          wx /= 30;
          wy /= 30;
          UIntVector pos2;
          u8 i = 0;
          while (Touch::IsDown())
          {
            std::string key;
            pos2 = Touch::GetPosition();
            Controller::Update();
            u16 sx = 120 + wx * 30;
            u16 sy = 70 + wy * 30;
            int posX = pos.x - pos2.x, posY = pos.y - pos2.y;
            if (posX > 10)
            {
              i = 1;
              scr.DrawRect(120 + wx * 30 - 18, 70 + wy * 30 + 3, 20, 20, Color::White);
              key.clear();
              Process::ReadString((u32)&U16_ChrArray[(wy * 3 + wx) * 5 + 1], key, 2, StringFormat::Utf16);
              scr.DrawSysfont(key, sx - 15, sy + 3, Color::Black);
            }
            else if (posY > 10)
            {
              i = 2;
              scr.DrawRect(120 + wx * 30 + 2, 70 + wy * 30 - 17, 20, 20, Color::White);
              key.clear();
              Process::ReadString((u32)&U16_ChrArray[(wy * 3 + wx) * 5 + 2], key, 2, StringFormat::Utf16);
              scr.DrawSysfont(key, sx + 5, sy - 17, Color::Black);
            }
            else if (posX < -10)
            {
              i = 3;
              scr.DrawRect(120 + wx * 30 + 22, 70 + wy * 30 + 3, 20, 20, Color::White);
              key.clear();
              Process::ReadString((u32)&U16_ChrArray[(wy * 3 + wx) * 5 + 3], key, 2, StringFormat::Utf16);
              scr.DrawSysfont(key, sx + 25, sy + 3, Color::Black);
            }
            else if (posY < -10)
            {
              i = 4;
              scr.DrawRect(120 + wx * 30 + 2, 70 + wy * 30 + 23, 20, 20, Color::White);
              key.clear();
              Process::ReadString((u32)&U16_ChrArray[(wy * 3 + wx) * 5 + 4], key, 2, StringFormat::Utf16);
              scr.DrawSysfont(key, sx + 5, sy + 23, Color::Black);
            }
            else
            {
              i = 0;
              scr.DrawRect(120 + wx * 30 + 2, 70 + wy * 30 + 3, 20, 20, Color::White);
              key.clear();
              Process::ReadString((u32)&U16_ChrArray[(wy * 3 + wx) * 5], key, 2, StringFormat::Utf16);
              scr.DrawSysfont(key, sx + 5, sy + 3, Color::Black);
              scr.DrawRect(120 + wx * 30 - 18, 70 + wy * 30 + 3, 20, 20, Color::White);
              key.clear();
              Process::ReadString((u32)&U16_ChrArray[(wy * 3 + wx) * 5 + 1], key, 2, StringFormat::Utf16);
              scr.DrawSysfont(key, sx - 15, sy + 3, Color::Black);
              scr.DrawRect(120 + wx * 30 + 2, 70 + wy * 30 - 17, 20, 20, Color::White);
              key.clear();
              Process::ReadString((u32)&U16_ChrArray[(wy * 3 + wx) * 5 + 2], key, 2, StringFormat::Utf16);
              scr.DrawSysfont(key, sx + 5, sy - 17, Color::Black);
              scr.DrawRect(120 + wx * 30 + 22, 70 + wy * 30 + 3, 20, 20, Color::White);
              key.clear();
              Process::ReadString((u32)&U16_ChrArray[(wy * 3 + wx) * 5 + 3], key, 2, StringFormat::Utf16);
              scr.DrawSysfont(key, sx + 25, sy + 3, Color::Black);
              scr.DrawRect(120 + wx * 30 + 2, 70 + wy * 30 + 23, 20, 20, Color::White);
              key.clear();
              Process::ReadString((u32)&U16_ChrArray[(wy * 3 + wx) * 5 + 4], key, 2, StringFormat::Utf16);
              scr.DrawSysfont(key, sx + 5, sy + 23, Color::Black);
            }
            OSD::SwapBuffers();
            DrawKeyboard(scr);
          }
          if (!InputChrs.empty())
          {
            if (U16_ChrArray[(wy * 3 + wx) * 5 + i] == 0x309B)
              Dakuten(false, InputChrs[InputChrs.size() - cursorPos - 1]);
            else if (U16_ChrArray[(wy * 3 + wx) * 5 + i] == 0x5C0F)
              Komoji(InputChrs[InputChrs.size() - cursorPos - 1]);
            else if (U16_ChrArray[(wy * 3 + wx) * 5 + i] == 0x309C)
              Dakuten(true, InputChrs[InputChrs.size() - cursorPos - 1]);
            else if (U16_ChrArray[(wy * 3 + wx) * 5 + i] == 0x5927)
              Komoji(InputChrs[InputChrs.size() - cursorPos - 1]);
          }
          if (InputChrs.size() < _maxLength && (U16_ChrArray[(wy * 3 + wx) * 5 + i] != 0x309B && U16_ChrArray[(wy * 3 + wx) * 5 + i] != 0x5C0F && U16_ChrArray[(wy * 3 + wx) * 5 + i] != 0x309C && U16_ChrArray[(wy * 3 + wx) * 5 + i] != 0x5927))
            InputChrs.insert(InputChrs.end() - cursorPos, U16_ChrArray[(wy * 3 + wx) * 5 + i]);

          selectedIndex = 0;
        }
      }
      else if (pos.x >= 23 && pos.y >= 69 && pos.x <= 262 && pos.y <= 178)
      {
        int wx = pos.x - 23;
        int wy = pos.y - 69;
        wx /= 24;
        wy /= 22;
        scr.DrawRect(23 + wx * 24, 68 + wy * 22, 24, 22, Color::White);

        if (InputChrs.size() < _maxLength)
          InputChrs.insert(InputChrs.end() - cursorPos, U16_ChrArray[wy * 10 + wx]);

        selectedIndex = 0;
      }
    }

    return true;
  }

  bool JPKeyboard::Open(std::string &out)
  {
    if (!Process::IsPaused())
      return false;

    InputChrs.clear();
    InputStr.clear();

    const Screen &topScr = OSD::GetTopScreen();
    const Screen &scr = OSD::GetBottomScreen();

    KeyboardOpened = true;
    KatakanaMode = false;
    while (KeyboardOpened)
    {
      topScr.DrawRect(30, 20, 340, 200, Color::Black);
      topScr.DrawRect(32, 22, 336, 196, Color::White, false);
      topScr.DrawSysfont(_text, 35, 25);
      Controller::Update();
      if (!DrawKeyboard(scr) || !InputKeyboard(scr, out))
        return false;
      OSD::SwapBuffers();
    }
    return true;
  }

  bool JPKeyboard::Open(std::string &out, std::string defaultText)
  {
    if (!Process::IsPaused())
      return false;

    InputChrs.clear();
    InputStr.clear();

    Process::WriteString((u32)U16_ChrArray, defaultText.substr(0, std::min(size_t(_maxLength) * 3, size_t(180))), StringFormat::Utf16);
    for (int i = 0; i < Convert::getMultiByte(defaultText.substr(0, std::min(size_t(_maxLength) * 3, size_t(180)))); i++)
    {
      InputChrs.emplace_back(U16_ChrArray[i]);
    }

    const Screen &topScr = OSD::GetTopScreen();
    const Screen &scr = OSD::GetBottomScreen();

    KeyboardOpened = true;
    KatakanaMode = false;
    while (KeyboardOpened)
    {
      topScr.DrawRect(30, 20, 340, 200, Color::Black);
      topScr.DrawRect(32, 22, 336, 196, Color::White, false);
      topScr.DrawSysfont(_text, 35, 25);
      Controller::Update();
      if (!DrawKeyboard(scr) || !InputKeyboard(scr, out))
        return false;
      OSD::SwapBuffers();
    }
    return true;
  }
}