#pragma once

#include <string>

#define CURL_STATICLIB
#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
bool http_get(std::string url, std::string& str);
bool http_post(std::string url, std::string content, std::string& str);
bool http_delete(std::string url, std::string& str);
