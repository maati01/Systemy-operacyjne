CC = gcc -Wall

run:
	$(CC) -o main main.c
	echo "Library version times:">>./pomiar_zad_2a.txt
	make run_test

run_test:
	time -p ./main b test.txt 2>>./pomiar_zad_2a.txt 
	cat pomiar_zad_2a.txt 
clean:
	rm -f *.o *.a *.so main