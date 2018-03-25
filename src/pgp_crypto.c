#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pgp_crypto.h"

//recipient must be string.
int pgp_enc(char *input_file_name, char *recipient, char *output_file_name) {
  int ret = -1;
  FILE *file = NULL;
  int cnt = 0;
  int recipient_len = 0;
	int input_file_name_len = 0;
	int output_file_name_len = 0;

  recipient_len = strlen(recipient);
	input_file_name_len = strlen(input_file_name);
	output_file_name_len = strlen(output_file_name);

  char *buff = malloc(sizeof(char) * strlen("OK"));
  if (!buff) {
    perror("malloc");
    goto EXIT;
  }
  memset(buff, 0, sizeof(char) * strlen("OK"));

  char *command = malloc(sizeof(char) * (input_file_name_len + recipient_len + output_file_name_len + 200));
  if (!command) {
    perror("malloc");
    goto EXIT;
  }
  memset(command, 0, sizeof(char) * (input_file_name_len + recipient_len + output_file_name_len + 200));
  //printf("malloc size:%zu\n",sizeof(char) * (input_file_name_len + recipient_len + output_file_name_len + 200));

  strncat(command, "gpg ", strlen("gpg ") + 1);
  strncat(command, "-o \"", strlen("-o \"") + 1);
  strncat(command, output_file_name, output_file_name_len + 1);
  strncat(command, "\" ", strlen("\" ") + 1);
  strncat(command, "--encrypt --armor --yes -r \"", strlen("--encrypt --armor --yes -r \"") + 1);
  strncat(command, recipient, recipient_len + 1);
  strncat(command, "\" ", strlen("\" ") + 1);
  strncat(command, "\"", strlen("\"") + 1);
  strncat(command, input_file_name, input_file_name_len + 1);
  strncat(command, "\" ", strlen("\" ") + 1);
  strncat(command, "2>/dev/null ", strlen("2>/dev/null ") + 1);
  strncat(command, "&& echo -n \"OK\" || echo -n \"KO\"", strlen("&& echo -n \"OK\" || echo -n \"KO\"") + 1);
  //strncat(command, " >> encrypted.asc", 100); // TEMp
	//printf("%s\n",command);

  file = popen(command, "r");
  if (!file) {
    perror("popen");
    goto EXIT;
  }

  cnt = fread(buff, sizeof(char), strlen("OK"), file);
  //printf("read_length:%d, string: %s\n",cnt,buff);

  //Check it is encrypted well
  if (cnt == 2) {
    if (strncmp(buff, "OK", strlen("OK")) == 0) {
      ret = 0;
    }
    else if (strncmp(buff, "KO", strlen("KO")) == 0) {
      ret = -1;
    }
  }
  //printf("read_length:%d\n",cnt);
  //printf("%s\n",*plain);

EXIT:
  if (file)
    pclose(file);
  if (command)
    free(command);
  if (buff)
    free(buff);

  return ret;
}

// passphrase must be string
int pgp_dec(char *input_file_name, char *passphrase, char *output_file_name) {
  int ret = -1;
  FILE *file = NULL;
  int cnt = 0;
  int passphrase_len = 0;
	int input_file_name_len = 0;
	int output_file_name_len = 0;

  passphrase_len = strlen(passphrase);
	input_file_name_len = strlen(input_file_name);
	output_file_name_len = strlen(output_file_name);
  
	char *buff = malloc(sizeof(char) * strlen("OK"));
  if (!buff) {
    perror("malloc");
    goto EXIT;
  }
  memset(buff, 0, sizeof(char) * strlen("OK"));

  char *command = malloc(sizeof(char) * (input_file_name_len + passphrase_len + output_file_name_len + 200));
  if (!command) {
    perror("malloc");
    goto EXIT;
  }
  memset(command, 0, sizeof(char) * (input_file_name_len + passphrase_len + output_file_name_len + 200));
  //printf("malloc size:%zu\n",sizeof(char) * (input_file_name_len + passphrase_len + output_file_name_len + 200));

  strncat(command, "gpg ", strlen("gpg ") + 1);
  strncat(command, "-o \"", strlen("-o \"") + 1);
  strncat(command, output_file_name, output_file_name_len + 1);
  strncat(command, "\" ", strlen("\" ") + 1);
  strncat(command, "--batch --yes --no-use-agent --passphrase \"", strlen("--batch --yes --no-use-agent --passphrase \"") + 1);
  strncat(command, passphrase, passphrase_len + 1);
  strncat(command, "\" ", strlen("\" ") + 1);
	strncat(command, "\"", strlen("\"") + 1);
  strncat(command, input_file_name, input_file_name_len + 1);
  strncat(command, "\" ", strlen("\" ") + 1);
  strncat(command, "2>/dev/null ", strlen("2>/dev/null ") + 1);
  strncat(command, "&& echo -n \"OK\" || echo -n \"KO\"", strlen("&& echo -n \"OK\" || echo -n \"KO\"") + 1);
	//printf("%s\n",command);

  file = popen(command, "r");
  if (!file) {
    perror("popen");
    goto EXIT;
  }

  cnt = fread(buff, sizeof(char), strlen("OK"), file);
  //printf("read_length:%d, string: %s\n",cnt,buff);

  //Check it is decrypted well
  if (cnt == 2) {
    if (strncmp(buff, "OK", strlen("OK")) == 0) {
      ret = 0;
    }
    else if (strncmp(buff, "KO", strlen("KO")) == 0) {
      ret = -1;
    }
  }

EXIT:
  if (file)
    pclose(file);
  if (command)
    free(command);
  if (buff)
    free(buff);

  return ret;
}

