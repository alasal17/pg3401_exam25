CC = gcc
CFLAGS = -Wall -O2 -pthread

all: task4_threads

task4_threads: task4_threads.c dbj2.c tea.c
	$(CC) $(CFLAGS) -o task4_threads task4_threads.c dbj2.c tea.c

clean:
	rm -f task4_threads *.o task4_pg2265.hash task4_pg2265.enc
