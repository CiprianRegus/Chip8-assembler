CC = gcc
CFLAGS = -Wall
INPUT_FILE = tf

build:
	$(CC) $(CFLAGS) -o a.out lexer.c

run: build
	./a.out $(INPUT_FILE)
	
clean:
	rm -f a.out