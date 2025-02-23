threaded_sum: threaded_sum.o
	gcc -o threaded_sum threaded_sum.o

threaded_sum.o: threaded_sum.c
	gcc -c -pthread -Wall threaded_sum.c

clean:
	rm *.o threaded_sum