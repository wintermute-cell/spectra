CC=gcc
CFLAGS=-lSDL2 -I.

spectra: spectra.c
	$(CC) -o spectra spectra.c $(CFLAGS)
