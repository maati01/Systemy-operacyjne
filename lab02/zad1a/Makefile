CC = gcc -Wall

run:
	$(CC) -o main main.c
	echo "Library version times:">>./pomiar_zad_1a.txt
	make run_test

run_test:
	time -p ./main test.txt result.txt 2>>./pomiar_zad_1a.txt 
	cat pomiar_zad_1a.txt 
clean:
	rm -f *.o *.a *.so main