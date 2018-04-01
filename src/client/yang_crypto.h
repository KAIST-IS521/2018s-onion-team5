#pragma once

#include <string>
#include <vector>

int dec(
  std::string input_file,
  std::string my_github_id,
  std::string passphrase,
	int *type,
  std::string& output_file,
  std::string& github_id
);

std::string enc(
  std::string input_file,
  std::vector<std::string> path,
	int type
);
