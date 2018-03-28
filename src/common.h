#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>

#define IN
#define OUT

int get_file_size(IN FILE *fp);
char *rand_string(IN char *str, IN size_t size);

#endif
