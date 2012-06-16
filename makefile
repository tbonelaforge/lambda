all: list hash lambda2

lambda2: lambda2.c lambda_expr.o lambda_parser2.o ./hash/hash.a ./list/list.o
	gcc -I ./hash -I ./list lambda2.c \
	 ./hash/hash.a ./list/list.o lambda_expr.o lambda_parser2.o \
	-o lambda2

lambda_expr.o: lambda_expr.c lambda_expr.h
	gcc -I ./hash -I ./list -c lambda_expr.c -o lambda_expr.o

lambda_parser2.o: lambda_parser.c lambda_parser.h
	gcc -I ./hash -I ./list -c lambda_parser2.c -o lambda_parser2.o

.PHONY: list

list: 
	cd ./list; make

.PHONY: hash

hash:
	cd ./hash; make