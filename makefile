all: list hash lambda

lambda: lambda.c lambda_expr.o lambda_parser.o \
	./hash/hash.o ./hash/charhash.o ./hash/hash_default.o \
	 ./list/list.o
	gcc -I ./hash -I ./list lambda.c \
	 ./hash/hash.o ./hash/charhash.o ./hash/hash_default.o \
	 ./list/list.o lambda_expr.o lambda_parser.o \
	-o lambda

lambda_expr.o: lambda_expr.c lambda_expr.h
	gcc -I ./hash -I ./list -c lambda_expr.c -o lambda_expr.o

lambda_parser.o: lambda_parser.c lambda_parser.h
	gcc -I ./hash -I ./list -c lambda_parser.c -o lambda_parser.o

clean:
	find ./ -name '*.o' | xargs rm; rm lambda

.PHONY: list

list: 
	cd ./list; make

.PHONY: hash

hash:
	cd ./hash; make
