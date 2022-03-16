CC = gcc 
LIB_O = library.o
LIB_A = library.a
LIB_C = library.c


static:
	$(CC) -c $(LIB_C)
	ar cr liblibrary.a $(LIB_O)
	$(CC) main.c -o main -L. -l library

shared:
	$(CC) -fPIC -c $(LIB_C)
	$(CC) -shared -fPIC -o liblibrary.so $(LIB_O)
	$(CC) main.c -o main -L. -l library

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
