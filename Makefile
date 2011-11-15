CC     = gcc
CFLAGS = -O2 -fomit-frame-pointer -funroll-loops -fstrict-aliasing
WFLAGS = -Wall -Wextra -pedantic -std=c99
LFLAGS = -lcrypto

trip	:	trip.c
	$(CC) $(CFLAGS) $(WFLAGS) $(LFLAGS) trip.c -o trip

clean	:
	rm -f trip

install	:	trip
	cp trip /usr/bin/

uninstall:
	rm -f /usr/bin/trip
