#include "common.h"

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
