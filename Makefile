CC	= gcc
CFLAGS	= -Wall -Wextra -pedantic -std=c99 -lssl -O3

all: trip

clean:
	rm -f trip

.PHONY: clean
