CC = gcc -Wall

run_server:
	$(CC) -o server server.c
	./server

run_client:
	$(CC) -o client client.c
	./client

clean:
	rm -f *.o *.a *.so main execl *.txt

clean_txt:
	rm *.txt