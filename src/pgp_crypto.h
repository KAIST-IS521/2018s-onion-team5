
int pgp_enc(char *plain, int plain_size, char *recipient, char **ciphered);
int pgp_dec(char *ciphered, int ciphered_size, char *passphrase, char **plain);