#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define char_len 10

static char *rand_string(char *str, size_t size)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK.0123456789";
	if (size) {
		--size;
		for (size_t n = 0; n < size; n++) {
			int key = rand() % (int) (sizeof charset - 1);
			str[n] = charset[key];
		}
		str[size] = '\0';
	}
	return str;
}

int main() {
	char *buff = malloc(sizeof(char) * char_len);
	if (!buff) {
		perror("malloc");
		exit(-1);
	}
	memset(buff, 0 ,sizeof(char) * char_len);
	srand(time(NULL));
	for (int i = 0; i< 10; i++) {
		buff = rand_string(buff, char_len);
		printf("%s\n",buff);
	}

	return 0;
}
