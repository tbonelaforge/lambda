all: hash.o charhash.o hash_default.o

charhash.o: charhash.c charhash.h
	gcc -I . -c charhash.c -o charhash.o

hash.o: hash.c hash.h charhash.h
	gcc -I . -I ../list -c hash.c -o hash.o

hash_default.o: hash_default.c hash_default.h
	gcc -I . -I ../list -c hash_default.c -o hash_default.o