CC = gcc -Wall
run:
	$(CC) -g main.c -lm -o main
	make run_test

run_test:
	./main 0.01 10000
	./main 0.001 25000
	./main 0.0001 15000
	make clean_txt

clean:
	rm -f *.o *.a *.so main w*.txt

clean_txt:
	rm w*.txt