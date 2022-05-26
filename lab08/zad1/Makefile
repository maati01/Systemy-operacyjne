CC = gcc -Wall

run:
	$(CC) -o main main.c -lpthread
	./main 1 numbers saturn.ascii.pgm result.ascii.pgm
	./main 2 numbers saturn.ascii.pgm result.ascii.pgm
	./main 4 numbers saturn.ascii.pgm result.ascii.pgm
	./main 8 numbers saturn.ascii.pgm result.ascii.pgm
	./main 16 numbers saturn.ascii.pgm result.ascii.pgm
	./main 1 block saturn.ascii.pgm result.ascii.pgm
	./main 2 block saturn.ascii.pgm result.ascii.pgm
	./main 4 block saturn.ascii.pgm result.ascii.pgm
	./main 8 block saturn.ascii.pgm result.ascii.pgm
	./main 16 block saturn.ascii.pgm result.ascii.pgm

clean:
	rm -f *.o *.a *.so main