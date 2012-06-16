all: lambda2

lambda2: lambda_expr.o lambda_parser2.o
	gcc -I ./hash -I ./list lambda2.c ./hash/hash.a ./list/list.o lambda_expr.o lambda_parser2.o -o lambda2

lambda_expr.o:
	gcc -I ./hash -I ./list -c lambda_expr.c -o lambda_expr.o

lambda_parser2.o:
	gcc -I ./hash -I ./list -c lambda_parser2.c -o lambda_parser2.o