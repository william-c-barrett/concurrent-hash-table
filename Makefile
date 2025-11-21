CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE -pthread -g
TARGET = chash
SOURCES = chash.c hash.c rwlock.c hash_table.c logger.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = hash.h rwlock.h hash_table.h logger.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) hash.log

test: $(TARGET)
	./$(TARGET)
