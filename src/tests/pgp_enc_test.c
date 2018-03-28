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

void usage(char *prog) {
  printf("Usage: %s [fro] -f \"input_file\" -r recipient -o \"output_file\"\n", prog);
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "fro")) != -1) {
    switch (opt) {
    case 'f':
      input_file_name = argv[optind];
      break;
    case 'r':
      recipient = argv[optind];
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
/*
void base_init() {
	ciphered = malloc(sizeof(char)*1024*1024*1024);
	if (!ciphered) {
		perror("malloc");
		exit(-1);
	}
	memset(ciphered, 0, sizeof(char) * 1024*1024*1024);

	buff = malloc(sizeof(char) * 1024*1024*1024);
	if (!buff) {
		perror("malloc");
		exit(-1);
	}
	memset(buff, 0, sizeof(char) * 1024*1024*1024);

	if(input_file_name) {
		fp = fopen(input_file_name, "rb");
		if (!fp) {
			perror("fopen");
			exit(-1);
		}
	}	
}

void base_free() {
	if (ciphered)
		free(ciphered);
	if (buff)
		free(buff);
	if (fp)
		fclose(fp);
}
*/

int main(int argc, char **argv) {
	int ret = -1;	

	parse_args(argc, argv);
	//base_init();
	ret = pgp_enc(input_file_name, recipient, output_file_name);
	if (ret == 0) {
		printf("Encryption Success\n");
	}
	else {
		printf("Encryption failed\n");
	}
	//base_free();

	return 0;
}
