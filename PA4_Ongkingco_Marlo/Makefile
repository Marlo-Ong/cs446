mymalloc: mymalloc.o
	gcc -o mymalloc mymalloc.o

mymalloc.o: mymalloc.c
	gcc -c -pthread -Wall -pedantic mymalloc.c

clean:
	rm *.o mymalloc