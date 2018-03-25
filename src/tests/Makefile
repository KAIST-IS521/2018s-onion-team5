all: pgp_enc_test pgp_dec_test pgp_enc_dec_test

pgp_enc_test : pgp_enc_test.o ../pgp_crypto.o ../common.o
	gcc -Wall -o pgp_enc_test pgp_enc_test.c ../pgp_crypto.c ../common.c

pgp_dec_test : pgp_dec_test.o ../pgp_crypto.o ../common.o
	gcc -Wall -o pgp_dec_test pgp_dec_test.c ../pgp_crypto.c ../common.c

pgp_enc_dec_test : pgp_enc_dec_test.o ../pgp_crypto.o ../common.o
	gcc -Wall -o pgp_enc_dec_test pgp_enc_dec_test.c ../pgp_crypto.c ../common.c

clean :
	rm ../*.o *.o pgp_enc_test pgp_dec_test pgp_enc_dec_test
