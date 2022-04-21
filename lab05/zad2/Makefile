CC = gcc -Wall
run:
	$(CC) -o main main.c

test:
	./main date
	./main sender
	./main "osboxes" "Hello" "Test email"

clean:
	rm -f *.o *.a *.so main execl *.txt

clean_txt:
	rm *.txt