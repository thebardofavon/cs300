CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS =

SOURCES = src/key.c src/bptree.c src/hashtable.c src/database.c main.c
OBJECTS = $(SOURCES:.c=.o)

TARGET = database_program

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean