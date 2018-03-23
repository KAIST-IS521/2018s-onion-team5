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
