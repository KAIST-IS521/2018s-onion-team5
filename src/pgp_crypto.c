#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pgp_crypto.h"

//recipient must be string.
//plain cannot be '\0'
int pgp_enc(char *plain, int plain_size, char *recipient, char **ciphered) {
  int ret = -1;
  FILE *file = NULL;
  int cnt = 0;
  int recipient_len = 0;

  /*
  if (plain[0] == '\0')
    goto EXIT;
  */
  recipient_len = strlen(recipient);

  char *command = malloc(sizeof(char) * (plain_size + recipient_len + 200));
  if (!command) {
    perror("malloc");
    goto EXIT;
  }
  memset(command, 0, sizeof(char) * (plain_size + recipient_len + 200));
  //printf("malloc size:%zu\n",sizeof(char) * (plain_size + recipient_len + 100));

  strncat(command, "echo -n \"", strlen("echo -n \"") + 1);
  strncat(command, plain, plain_size + 1);
  strncat(command, "\" ", strlen("\" ") + 1);
  strncat(command, "| gpg --encrypt --armor -r \"", strlen("| gpg --encrypt --armor -r \"") + 1);
  strncat(command, recipient, recipient_len + 1);
  strncat(command, "\"", strlen("\"") + 1);
  //strncat(command, " >> encrypted.asc", 100); // TEMp

  file = popen(command, "r");
  if (!file) {
    perror("popen");
    goto EXIT;
  }

  cnt = fread(*ciphered, sizeof(char), 1024*1024*1024, file);
  //printf("read_length:%d\n",cnt);
  //printf("%s\n",*ciphered);

  //Check it is encrypted well
  if (strstr(*ciphered, "-----BEGIN PGP MESSAGE-----") && strstr(*ciphered, "-----END PGP MESSAGE-----")) {
    ret = 0;
  }

EXIT:
  if (file)
    pclose(file);
  if (command)
    free(command);

  return ret;
}

// passphrase must be string
// success -> outputs something
// failed -> outputs nothing
int pgp_dec(char *ciphered, int ciphered_size, char *passphrase, char **plain) {
  int ret = -1;
  FILE *file = NULL;
  int cnt = 0;
  int passphrase_len = strlen(passphrase);

  char *command = malloc(sizeof(char) * (ciphered_size + passphrase_len + 200));
  if (!command) {
    perror("malloc");
    goto EXIT;
  }
  memset(command, 0, sizeof(char) * (ciphered_size + passphrase_len + 200));
  //printf("malloc size:%zu\n",sizeof(char) * (ciphered_size + passphrase_len + 100));

  strncat(command, "echo -n \"", strlen("echo -n \"") + 1);
  //strncat(command, "printf -- \"", strlen("printf -- \"") + 1);
  strncat(command, ciphered, ciphered_size + 1);
  strncat(command, "\" ", strlen("\" ") + 1);
  strncat(command, "| gpg --batch --no-use-agent --passphrase \"", strlen("| gpg --batch --no-use-agent --passphrase \"") + 1);
  strncat(command, passphrase, passphrase_len + 1);
  strncat(command, "\" ", strlen("\" ") + 1);
  strncat(command, "2>/dev/null ", strlen("2>/dev/null ") + 1);
  strncat(command, "&& echo -n \"OK\" || echo -n \"KO\"", strlen("&& echo -n \"OK\" || echo -n \"KO\"") + 1);
  //strncat(command, "&& printf \"OK\" || printf \"KO\"", strlen("&& printf \"OK\" || printf \"KO\"") + 1);
  //strncat(command, " >> encrypted.asc", 100); // TEMp

  file = popen(command, "r");
  if (!file) {
    perror("popen");
    goto EXIT;
  }

  cnt = fread(*plain, sizeof(char), 1024*1024*1024, file);

  //printf("hohoho :%s\n",*plain + (cnt-2));
  //printf("askl :%x\n",(*plain)[cnt]);
  if (cnt >= 2) {
    if (strncmp(*plain + cnt-2, "OK",strlen("OK")) == 0) {
      ret = 0;
      memset(*plain + cnt-2, 0, strlen("OK") + 1);
    }
    else if (strncmp(*plain + cnt-2, "KO",strlen("KO")) == 0) {
      ret = -1;
      memset(*plain + cnt-2, 0, strlen("KO") + 1);
    }
  }
  //printf("read_length:%d\n",cnt);
  //printf("%s\n",*plain);

EXIT:
  if (file)
    pclose(file);
  if (command)
    free(command);

  return ret;
}

