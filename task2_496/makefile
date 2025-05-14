CC = gcc
CFLAGS = -Wall -O2
OBJ = main.o task2_hash.o task2_count.o task2_sum.o
TARGET = task2_exec

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c task2_hash.h task2_count.h task2_sum.h

task2_hash.o: task2_hash.c task2_hash.h

task2_count.o: task2_count.c task2_count.h

task2_sum.o: task2_sum.c task2_sum.h

clean:
	rm -f *.o $(TARGET) pgexam25_output.bin
