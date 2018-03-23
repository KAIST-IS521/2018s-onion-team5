#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "../common.h"
#include "../pgp_crypto.h"

char *output_file_name = NULL;
char *input_file_name = NULL;
char *plain = NULL;
char *recipient = NULL;

FILE *fp = NULL;
char *ciphered = NULL;
char *buff = NULL;

void usage(char *prog) {
  printf("Usage: %s [mfo] -m \"plain\" | -f \"file path\" -r recipient\n", prog);
  printf("\t-o output_file [Default stdout]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "mfor")) != -1) {

    switch (opt) {
    case 'm':
      plain = argv[optind];
      break;
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
  if (!input_file_name && !plain) {
    usage(argv[0]);
    exit(-1);
  }
  if (input_file_name && plain) {
    usage(argv[0]);
    exit(-1);
  }
  if (!recipient) {
    usage(argv[0]);
    exit(-1);
  }
}

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

int main(int argc, char **argv) {
	int f_size = -1;
	int cnt = -2;
	int ret = -1;	
	int plain_len = -1;
	/*
	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}
	*/
	parse_args(argc, argv);
	base_init();

	if (plain) {
		plain_len = strlen(plain);
		printf("plain_len : %d\n",plain_len);
		ret = pgp_enc(plain, plain_len, recipient, &ciphered);
		if (ret == 0) {
			printf("Encryption Success\n");
			if (output_file_name) {
				FILE *fp_w = fopen(output_file_name,"w");
				fputs(ciphered,fp_w);
				fclose(fp_w);
			}
			else {
				printf("%s\n",ciphered);
			}
		}
		else {
			printf("Encryption failed\n");
		}
	}

	if (input_file_name) {
		f_size = get_file_size(fp);
		cnt = fread(buff, sizeof(char), f_size, fp);
		if (f_size == cnt) {
			plain_len = f_size;
			ret = pgp_enc(buff, plain_len, recipient, &ciphered);
			if (ret == 0) {
				printf("Encryption Success\n");
				if (output_file_name) {
					FILE *fp_w = fopen(output_file_name,"w");
					fputs(ciphered,fp_w);
					fclose(fp_w);
				}
				else {
					printf("%s\n",ciphered);
				}
			}
			else {
				printf("Encryption failed\n");
			}
		}
	}	

EXIT:
	base_free();

	return 0;
}
