#pragma once

#include <map>
#include <set>

#include "Curl.hpp"
#include "KaniCodes.hpp"
#include "cheats.hpp"

namespace CTRPluginFramework {

class Convert {
 public:
  static u16 sjisToUtf16(const u16 &sjis);
  static u16 utf16ToSjis(const u16 &utf16);
  static std::vector<u16> strToSjis(const std::string &str);
  static int getMultiByte(const std::string &str);
  static std::string toLower(std::string str);
  static std::string hiraganaToKanji(const std::string &hiragana);
  static std::string hiraganaToKatakana(const std::string &hiragana);
  static std::string katakanaToHiragana(const std::string &katakana);
  static void addHiraganaKanjiList(const std::string &hiragana);
  static void addHiraganaKanjiList(const std::string &hiragana, const std::string &kanji);

 private:
  static std::map<std::string, std::set<std::string>> hiragana_kanji_list;
  static std::set<std::string> done;
};
}  // namespace CTRPluginFramework
