#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "../common.h"
#include "../pgp_crypto.h"

char *output_file_name = NULL;
char *input_file_name = NULL;
char *passphrase = NULL;

void usage(char *prog) {
  printf("Usage: %s [fro] -f \"input_file\" -r passphrase -o \"output_file\"\n", prog);
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "fro")) != -1) {
    switch (opt) {
    case 'f':
      input_file_name = argv[optind];
      break;
    case 'r':
      passphrase = argv[optind];
      break;
    case 'o':
      output_file_name = argv[optind];
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (!input_file_name || !passphrase || !output_file_name) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char **argv) {
	int ret = -1;	

	parse_args(argc, argv);
	ret = pgp_dec(input_file_name, passphrase, output_file_name);
	if (ret == 0) {
		printf("Decryption Success\n");
	}
	else {
		printf("Decryption failed\n");
	}

	return 0;
}
