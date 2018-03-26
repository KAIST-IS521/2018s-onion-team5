#include "../common/config.h"
#include "fetch_key.h"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int fetch_key_from_github(std::string user_id, std::string& str) {
  CURL *curl;
  CURLcode res;
  int ret = 1;

  std::string url = "https://raw.githubusercontent.com/" + base_repo + "/2018-Spring/master/IndividualKeys/" + user_id + ".pub";

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
      ret = 0;
    } else {
      //fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
  }

  return ret;
}
