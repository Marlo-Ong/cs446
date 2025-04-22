myfs: myfs.o
	gcc -o myfs myfs.o

myfs.o: myfs.c
	gcc -c -pthread -Wall -pedantic myfs.c

clean:
	rm *.o myfs