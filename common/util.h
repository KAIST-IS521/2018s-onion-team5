#pragma once

#include <string>

int get_file_size(std::string locate);
std::string get_temppath();
std::string save_tempfile(std::string content);
bool write_file(std::string locate, std::string data);
bool read_file(std::string locate, std::string& data);
bool delete_file(std::string locate);
std::string random_string(size_t length);
