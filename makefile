# Makefile for Windows
SRCDIR=./source
CC=gcc
CFLAGS = -g -c 
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_net -lSDL2_ttf #-mwindows -lm

all:
	@echo "Building test"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c 
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c 
	$(CC) main.o init.o menu.o -o main.exe $(LDFLAGS)

clean:
	rm *.exe
	rm *.o
