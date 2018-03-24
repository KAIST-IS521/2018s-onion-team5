#pragma once

#include <string>

#define CURL_STATICLIB
#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

class HTTP{
public:
    static bool _get(std::string url);
    static bool _get(std::string url, std::string& str);
    static bool _post(std::string url, std::string content);
    static bool _post(std::string url, std::string content, std::string& str);
    static bool _delete(std::string url);
    static bool _delete(std::string url, std::string& str);
};
