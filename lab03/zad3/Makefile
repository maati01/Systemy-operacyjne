CC = gcc -Wall
run:
	$(CC) -o main main.c
	make run_test

run_test:
	./main test test 5
	./main test test 3
	./main test test 0

clean:
	rm -f *.o *.a *.so main w*.txt

clean_txt:
	rm w*.txt