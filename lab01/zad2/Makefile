CC = gcc 
LIB_O = library.o
LIB_A = library.a
LIB_C = library.c


static:
	$(CC) -c $(LIB_C)
	ar cr liblibrary.a $(LIB_O)
	$(CC) main.c -o run -L. -l library

shared:
	$(CC) -fPIC -c $(LIB_C)
	$(CC) -shared -fPIC -o liblibrary.so $(LIB_O)
	$(CC) main.c -o run -L. -l library

generate_test:
	head -c 1M /dev/random | base64 | head -c 1M > test1.txt
	head -c 8M /dev/random | base64 | head -c 8M > test2.txt
	head -c 32M /dev/random | base64 | head -c 32M > test3.txt
	head -c 64M /dev/random | base64 | head -c 64M > test4.txt
	head -c 128M /dev/random | base64 | head -c 128M > test5.txt
	
clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *.txt
	rm -f run

