VALGRIND_FLAGS=--leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2
CC = gcc
CFLAGS =-g -O0 -Wall -Werror -std=c99 -pedantic

all: valgrind

valgrind-ejemplo: ejemplo
	valgrind $(VALGRIND_FLAGS) ./ejemplo 2>&1

ejemplo: src/*.c ejemplo.c
	$(CC) $(CFLAGS) src/*.c ejemplo.c -o ejemplo 2>&1

pruebas: src/*.c pruebas.c
	$(CC) $(CFLAGS) src/*.c pruebas.c -o pruebas 2>&1

valgrind: pruebas
	valgrind $(VALGRIND_FLAGS) ./pruebas 2>&1


.PHONY: clean
clean:
	rm -f ejemplo
	rm -f pruebas

