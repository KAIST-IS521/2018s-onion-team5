#include "common.h"
#include <stdio.h>
/* Returns the size of the file */
int get_file_size(IN FILE *fp) {
  int size;

  if (fseek(fp, 0, SEEK_END)) {
    perror("fseek");
    return -1; 
  }

  size = ftell(fp);
  if (size == -1) {
    perror("ftell");
    return -1; 
  }

  if (fseek(fp, 0, SEEK_SET)) {
    perror("fseek");
    return -1; 
  }

  return size;
}

static char *rand_string(char *str, size_t size)
{
  const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK.0123456789";
  if (size) {
    --size;
    for (size_t n = 0; n < size; n++) {
      int key = rand() % (int) (sizeof charset - 1); 
      str[n] = charset[key];
    }   
    str[size] = '\0';
  }
  return str;
}

