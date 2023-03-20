# Makefile for Windows

# Location of where hello.c is stored in
SRCDIR=.

# Name of the compiler, GNU GCC in this case
CC=gcc

# Any include directories
# INCLUDE = ./lib/macos/include
INCLUDE = C:\msys2\mingw64\include

# Any flags for the compiler
#CFLAGS = -g -I$(INCLUDE) -c
CFLAGS = -g -c

# Any libraries to link with
#LIBS = C:\msys2\mingw64\lib

# Extra flags to give to compilers when they are supposed to invoke the linker
# NOTE ORDER OF THE FLAGS MATTERS!
LDFLAGS = -lmingw32 -lSDL2main -lSDL2net -lSDL2 -mwindows

hello:
	@echo "Building main"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c -o $@.o 
	$(CC) main.o -o main $(LDFLAGS)

clean:
	rm main
	rm main.o