#ifndef PGP_CRYPTO_H
#define PGP_CRYPTO_H

int pgp_enc(char *input_file_name, char *recipient, char *output_file_name);
int pgp_dec(char *ciphered, int ciphered_size, char *passphrase, char **plain);

#endif
