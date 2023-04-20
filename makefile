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
	$(CC) main.o init.o text.o player.o menu.o levelEditor.o -o main.exe $(LDFLAGS)

demo:
	@echo "Building demo"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c
	$(CC) $(CFLAGS) $(SRCDIR)/text.c  
	$(CC) $(CFLAGS) $(SRCDIR)/levelEditor.c 
	$(CC) main.o init.o text.o levelEditor.o -o main.exe $(LDFLAGS)

net:
	@echo "Building Networking"
	$(CC) $(CFLAGS) $(SRCDIR)/udpClient.c 
	$(CC) $(CFLAGS) $(SRCDIR)/udpServer.c 
	$(CC) udpClient.o -o udpClient.exe $(LDFLAGS)
	$(CC) udpServer.o -o udpServer.exe $(LDFLAGS)

netUDP:
	@echo "Building Networking"
	$(CC) $(CFLAGS) $(SRCDIR)/clientUDP.c 
	$(CC) $(CFLAGS) $(SRCDIR)/serverUDP.c 
	$(CC) serverUDP.o -o serverUDP.exe $(LDFLAGS)

mapC:
	@echo "Building MapCreator"
	$(CC) $(CFLAGS) $(SRCDIR)/mapCreator.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c 
	$(CC) mapCreator.o init.o -o mapCreator.exe $(LDFLAGS)

clean:
	rm *.exe
	rm *.o
