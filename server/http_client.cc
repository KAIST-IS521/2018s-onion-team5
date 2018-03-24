#include "http_client.h"

#define CURL_STATICLIB
#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool HTTP::_get(std::string url) {
  std::string dummy;
  return HTTP::_get(url, dummy);
}

bool HTTP::_get(std::string url, std::string& str) {
  CURL *curl;
  CURLcode res;
  int ret = false;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);

    res = curl_easy_perform(curl);

    if(res == CURLE_OK) {
      ret = true;
    } else {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
  }

  return ret;
}

bool HTTP::_post(std::string url, std::string content) {
  std::string dummy;
  return  HTTP::_post(url, content, dummy);
}

bool HTTP::_post(std::string url, std::string content, std::string& str) {
  CURL *curl;
  CURLcode res;
  int ret = false;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);

    res = curl_easy_perform(curl);

    if(res == CURLE_OK) {
      ret = true;
    } else {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
  }

  return ret;
}

bool HTTP::_delete(std::string url) {
  std::string dummy;
  return HTTP::_delete(url, dummy);
}

bool HTTP::_delete(std::string url, std::string& str) {
  CURL *curl;
  CURLcode res;
  int ret = false;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);

    res = curl_easy_perform(curl);

    if(res == CURLE_OK) {
      ret = true;
    } else {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
  }

  return ret;
}
