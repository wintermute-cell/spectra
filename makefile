CC=gcc
CFLAGS=-lSDL2 -lSDL2_image -I.

spectra: spectra.c
	$(CC) -o spectra spectra.c $(CFLAGS)
