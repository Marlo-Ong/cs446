sched: sched.o
	gcc -o sched sched.o

sched.o: sched.c
	gcc -c -pthread -Wall sched.c

clean:
	rm *.o sched