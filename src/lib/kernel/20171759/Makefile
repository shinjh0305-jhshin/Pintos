.SUFFIXES = .c .o

CC = gcc
CFLAGS = -Wall -g
TARGET = testlib
OBJS = list.o main.o hash.o bitmap.o hex_dump.o
SRCS = $(OBJS:.o=.c)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY : clean
clean :
	rm $(TARGET) $(OBJS)