CC = gcc
CFLAGS = -Wall
INPUT_FILE = tf

build:
	$(CC) $(CFLAGS) -o a lexer.c

run: build
	./a $(INPUT_FILE)
	
clean:
	rm -f a