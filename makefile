# Makefile for Windows
SRCDIR=./source
CC=gcc
CFLAGS = -g -c
LDFLAGS = -Wall

# Determine the operating system
ifeq ($(OS),Windows_NT)
	EXE_EXT = .exe
	LDFLAGS += -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_net -lSDL2_ttf -lSDL2_mixer -lpthread -liphlpapi -lws2_32
all:
	@echo "Building main"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c
	$(CC) $(CFLAGS) $(SRCDIR)/text.c
	$(CC) $(CFLAGS) $(SRCDIR)/player.c  
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c    
	$(CC) $(CFLAGS) $(SRCDIR)/levelEditor.c
	$(CC) $(CFLAGS) $(SRCDIR)/ioHandler.c
	$(CC) $(CFLAGS) $(SRCDIR)/newClient.c
	$(CC) $(CFLAGS) $(SRCDIR)/getDefaultGateway.c
	$(CC) main.o init.o text.o player.o menu.o levelEditor.o ioHandler.o newClient.o getDefaultGateway.o -o main$(EXE_EXT) $(LDFLAGS)
	@echo "Building Server"
	$(CC) $(CFLAGS) $(SRCDIR)/betterServer.c 
	$(CC) betterServer.o init.o text.o ioHandler.o player.o -o betterServer$(EXE_EXT) $(LDFLAGS)

launcher:
	@echo "Building GameLauncher"
	$(CC) $(CFLAGS) $(SRCDIR)/gameLauncher.c
	$(CC) gameLauncher.o -o launcher$(EXE_EXT) 

else
	EXE_EXT =
	LDFLAGS +=-lSDL2main -lSDL2_image -lSDL2 -lSDL2_net -lSDL2_ttf -lSDL2_mixer -lpthread -lm
all:
	@echo "Building main"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c
	$(CC) $(CFLAGS) $(SRCDIR)/text.c
	$(CC) $(CFLAGS) $(SRCDIR)/player.c  
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c    
	$(CC) $(CFLAGS) $(SRCDIR)/levelEditor.c
	$(CC) $(CFLAGS) $(SRCDIR)/ioHandler.c
	$(CC) $(CFLAGS) $(SRCDIR)/newClient.c
	$(CC) main.o init.o text.o player.o menu.o levelEditor.o ioHandler.o newClient.o -o main$(EXE_EXT) $(LDFLAGS)
	@echo "Building Server"
	$(CC) $(CFLAGS) $(SRCDIR)/betterServer.c 
	$(CC) betterServer.o init.o text.o ioHandler.o player.o -o betterServer$(EXE_EXT) $(LDFLAGS)
endif

clean:
	rm *.o
