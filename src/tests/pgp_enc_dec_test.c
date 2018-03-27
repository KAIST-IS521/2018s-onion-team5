#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "../common.h"
#include "../pgp_crypto.h"

char *output_file_name = NULL;
char *input_file_name = NULL;
char *recipient = NULL;
char *passphrase = NULL;
char *command = NULL;
int input_file_name_len = 0;
int output_file_name_len = 0;

void usage(char *prog) {
  printf("Usage: %s [frpo] -f \"input_file\" -r recipient -p passphrase -o \"output_file\"\n", prog);
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "frop")) != -1) {
    switch (opt) {
    case 'f':
      input_file_name = argv[optind];
      break;
    case 'r':
      recipient = argv[optind];
      break;
    case 'p':
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
  if (!input_file_name || !recipient || !output_file_name) {
    usage(argv[0]);
    exit(-1);
  }
}

void base_init() {
	input_file_name_len = strlen(input_file_name);
	output_file_name_len = strlen(output_file_name);

	command = (char *)malloc(sizeof(char) * (input_file_name_len + output_file_name_len + 100));
	if (!command) {
		perror("malloc");
		exit(-1);
	}
	memset(command, 0, sizeof(char) * (input_file_name_len + output_file_name_len + 100));
}

void base_free() {
	if (command)
		free(command);
}

int main(int argc, char **argv) {
	int ret = -1;	
	int ret2 = -1;

	parse_args(argc, argv);
	base_init();
	ret = pgp_enc(input_file_name, recipient, "this_is_a_temp_file");
	if (ret == 0) {
		printf("Encryption Success\n");
	}
	else {
		printf("Encryption failed\n");
	}
	ret2 = pgp_dec("this_is_a_temp_file", passphrase, output_file_name);
	if (ret2 == 0) {
		printf("Decryption Success\n");
	}
	else {
		printf("Decryption failed\n");
	}

	if( ret == 0 && ret2 == 0) {
		printf("Checking md5sum of two files...\n");
		strncat(command, "md5sum ", strlen("md5sum "));
		strncat(command, input_file_name, strlen(input_file_name));
		strncat(command, " ", strlen(" "));
		strncat(command, output_file_name, strlen(output_file_name));

		system(command);
	}
	remove("this_is_a_temp_file");
	base_free();

	return 0;
}
