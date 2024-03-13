#pragma once

#include <CTRPluginFramework.hpp>

#include "curl/curl.h"

namespace CTRPluginFramework {
class Curl {
 public:
  static CURLcode Get(const char *url, std::vector<u8> &out);
  static CURLcode Post(const char *url, const char *data, std::vector<u8> &out);
  static CURLcode Download(const char *url, const std::string &file_name);

 private:
  static File _file;

  static size_t callBack(u8 *ptr, size_t size, size_t nmemb, std::vector<u8> *stream);
  static size_t callBackDownload(void *ptr, size_t size, size_t nmemb, void *stream);
};
}  // namespace CTRPluginFramework
