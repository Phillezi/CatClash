# Makefile for Windows
SRCDIR=./source
CC=gcc
CFLAGS = -g -c 
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_net -mwindows -lm

all:
	@echo "Building test"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) main.o -o main.exe $(LDFLAGS)

clean:
	rm *.exe
	rm *.o
