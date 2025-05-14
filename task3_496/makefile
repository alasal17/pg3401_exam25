CC = gcc
CFLAGS = -Wall -Wextra -g
OBJDIR = obj
INCLDIR = .

SRCS = main.c task3.c
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

TARGET = task3_program

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR) *.o $(TARGET)
