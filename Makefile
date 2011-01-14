CC	= gcc
CFLAGS	= -Wall -Wextra -pedantic -std=c99 -lssl -O3

trip	:	trip.c
	$(CC) $(CFLAGS) trip.c -o trip

clean	:
	rm -f trip

install	:	trip
	cp trip /usr/bin/

uninstall:
	rm -f /usr/bin/trip