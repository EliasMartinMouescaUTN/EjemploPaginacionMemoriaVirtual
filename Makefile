CC = gcc
CFLAGS = -Wall -Wextra -O2

# Default targets
all: 32 64

32: main.c
	$(CC) $(CFLAGS) -m32 -o $@ $<

64: main.c
	$(CC) $(CFLAGS) -m64 -o $@ $<

clean:
	rm -f pages_32 pages_64

