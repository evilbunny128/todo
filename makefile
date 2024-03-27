
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SRCS = main.c
OBJS = $(SRCS:.c=.o)
TARGET = todo

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

