#include "Curl.hpp"

namespace CTRPluginFramework
{
  CURLcode Curl::Get(const char *url, std::vector<u8> &out)
  {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    std::vector<u8> chunk;
    char *location = nullptr;
    long response;

    if (!curl)
    {
      curl_easy_cleanup(curl);
      return CURL_LAST;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
    curl_easy_setopt(curl, CURLOPT_PROXY, "");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    do
    {
      if ((res = curl_easy_perform(curl)) != CURLE_OK)
        break;
      if ((res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response)) != CURLE_OK)
        break;
      if (response / 100 != 3)
        break;
      if ((res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &location)) != CURLE_OK)
        break;
      if (location)
        curl_easy_setopt(curl, CURLOPT_URL, location);
      MessageBox(std::string(location))();
    } while (response / 100 == 3);

    curl_easy_cleanup(curl);

    if (res == CURLE_OK)
      out = chunk;

    return res;
  }

  CURLcode Curl::Post(const char *url, const char *post_data, std::vector<u8> &out)
  {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    std::vector<u8> chunk;
    curl_slist *slist1 = nullptr;
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");

    if (!curl)
    {
      curl_easy_cleanup(curl);
      return CURL_LAST;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_data));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res == CURLE_OK)
      out = chunk;

    return res;
  }

  CURLcode Curl::Download(const char *url, const std::string &file_name)
  {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    std::vector<u8> chunk;
    char *location = nullptr;
    long response;
    File::Open(_file, file_name, File::Mode::RWC);

    if (!curl || !_file.IsOpen() || _file.Clear() != File::OPResult::SUCCESS)
    {
      curl_easy_cleanup(curl);
      _file.Close();
      return CURL_LAST;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callBackDownload);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (std::vector<u8> *)&chunk);
    curl_easy_setopt(curl, CURLOPT_PROXY, "");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    do
    {
      if ((res = curl_easy_perform(curl)) != CURLE_OK)
        break;
      if ((res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response)) != CURLE_OK)
        break;
      if (response / 100 != 3)
        break;
      if ((res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &location)) != CURLE_OK)
        break;
      if (location)
        curl_easy_setopt(curl, CURLOPT_URL, location);
    } while (response / 100 == 3);

    curl_easy_cleanup(curl);
    _file.Flush();
    _file.Close();

    return res;
  }

  size_t Curl::callBack(u8 *ptr, size_t size, size_t nmemb, std::vector<u8> *stream)
  {
    size_t _size = size * nmemb;
    stream->insert(stream->end(), ptr, ptr + _size);

    return _size;
  }

  size_t Curl::callBackDownload(void *ptr, size_t size, size_t nmemb, void *stream)
  {
    size_t _size = size * nmemb;
    _file.Write((void *)ptr, _size);

    return _size;
  }

  File Curl::_file;
}