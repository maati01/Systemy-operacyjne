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

run_static:
	make static
	make static_tests

run_shared:
	make shared
	make shared_tests

run_dynamic:
	$(CC) -fPIC -c $(LIB_C)
	$(CC) -shared -fPIC -o liblibrary.so $(LIB_O)
	$(CC) main.c -ldl -o run -I . -D DLL
	make dynamic_tests

static_tests:
	make generate_test
	make tests_path

shared_tests:
	make generate_test
	make tests_path

dynamic_tests:
	make generate_test
	make tests_path
	
tests_path:
	# small test
	LD_LIBRARY_PATH=./ ./run create_table 2 wc_files test1.txt test2.txt test3.t save_block 1 remove_block 1
	#medium test
	LD_LIBRARY_PATH=./ ./run create_table 500 wc_files test3.txt test4.txt test5.txt save_and_remove_blocks 230 save_block 1 
	# big test
	LD_LIBRARY_PATH=./ ./run create_table 1001 wc_files test5.txt test5.txt test5.txt test5.txt test5.txt test5.txt test5.txt save_block 1 remove_block 1 save_and_remove_blocks 1000

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
