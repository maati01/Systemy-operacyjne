CC = gcc -Wall

run:
	$(CC) -o main main.c
	echo "System version times:">>./pomiar_zad_2b.txt
	make run_test

run_test:
	time -p ./main b test.txt 2>>./pomiar_zad_2b.txt 
	cat pomiar_zad_2b.txt 
clean:
	rm -f *.o *.a *.so main