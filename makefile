# Makefile for Windows
SRCDIR=./source
CC=gcc
CFLAGS = -g -c 
LDFLAGS = -Wall -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_net -lSDL2_ttf #-mwindows -lm

all:
	@echo "Building main"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c
	$(CC) $(CFLAGS) $(SRCDIR)/text.c
	$(CC) $(CFLAGS) $(SRCDIR)/player.c  
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c    
	$(CC) $(CFLAGS) $(SRCDIR)/levelEditor.c
	$(CC) $(CFLAGS) $(SRCDIR)/ioHandler.c
	$(CC) $(CFLAGS) $(SRCDIR)/TCPclient.c 
	$(CC) main.o init.o text.o player.o menu.o levelEditor.o ioHandler.o TCPclient.o -o main.exe $(LDFLAGS)

demo:
	@echo "Building demo"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c
	$(CC) $(CFLAGS) $(SRCDIR)/text.c  
	$(CC) $(CFLAGS) $(SRCDIR)/levelEditor.c 
	$(CC) main.o init.o text.o levelEditor.o -o main.exe $(LDFLAGS)

net:
	@echo "Building Networking"
	$(CC) $(CFLAGS) $(SRCDIR)/server.c 
	$(CC) $(CFLAGS) $(SRCDIR)/text.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c
	$(CC) $(CFLAGS) $(SRCDIR)/TCPserver.c
	$(CC) $(CFLAGS) $(SRCDIR)/serverUDP.c 
	$(CC) server.o text.o serverUDP.o init.o TCPserver.o -o server.exe $(LDFLAGS)

netTCP:
	@echo "Building Experimental networking"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c
	$(CC) $(CFLAGS) $(SRCDIR)/text.c
	$(CC) $(CFLAGS) $(SRCDIR)/player.c  
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c    
	$(CC) $(CFLAGS) $(SRCDIR)/levelEditor.c
	$(CC) $(CFLAGS) $(SRCDIR)/ioHandler.c
	$(CC) $(CFLAGS) $(SRCDIR)/TCPclient.c 
	$(CC) main.o init.o text.o player.o menu.o levelEditor.o ioHandler.o TCPclient.o -o main.exe $(LDFLAGS)
	$(CC) $(CFLAGS) $(SRCDIR)/TCPserver.c 
	$(CC) TCPserver.o init.o -o TCPserver.exe $(LDFLAGS)

netUDP:
	@echo "Building Networking"
	$(CC) $(CFLAGS) $(SRCDIR)/serverUDP.c 
	$(CC) $(CFLAGS) $(SRCDIR)/text.c 
	$(CC) serverUDP.o text.o -o serverUDP.exe $(LDFLAGS)

mapC:
	@echo "Building MapCreator"
	$(CC) $(CFLAGS) $(SRCDIR)/mapCreator.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c 
	$(CC) mapCreator.o init.o -o mapCreator.exe $(LDFLAGS)

clean:
	rm *.exe
	rm *.o
