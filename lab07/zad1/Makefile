CC = gcc -Wall

run:
	@touch oven
	@touch table
	$(CC) -o cook cook.c
	$(CC) -o supplier supplier.c
	$(CC) -o main main.c
	./main 8 10

clean:
	rm -f *.o *.a *.so main cook oven supplier table