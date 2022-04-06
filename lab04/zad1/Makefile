CC = gcc -Wall
run:
	$(CC) -o main main.c
	$(CC) -o execl execl.c

tests:
	for com in ignore handler mask pending ; do \
		for opt in fork exec ; do \
			printf "$$com $$opt\n" >> raport2.txt ; \
			./main $$com $$opt >> raport2.txt ; \
		done ; \
	done

clean:
	rm -f *.o *.a *.so main execl *.txt

clean_txt:
	rm *.txt