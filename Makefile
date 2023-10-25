CC = gcc
CFLAGS = -Wall -Wextra -pedantic -pthread
DEPS = serlib.h
OBJ = serializer.o serlib.o

all: serializer clean run

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

serializer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean run

clean:
	rm -f $(OBJ)

run:
	./serializer
