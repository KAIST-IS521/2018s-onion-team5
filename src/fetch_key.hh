#pragma once

#include <iostream>
#include <string>

#define CURL_STATICLIB
#include <curl/curl.h>

int fetch_key_from_github(std::string user_id, std::string& str);
