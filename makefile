# Makefile for Windows
SRCDIR=./source
CC=gcc
CFLAGS = -g -c 
LDFLAGS = -Wall -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_net -lSDL2_ttf #-mwindows -lm

all:
	@echo "Building test"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c 
	$(CC) main.o init.o -o main.exe $(LDFLAGS)

demo:
	@echo "Building demo"
	$(CC) $(CFLAGS) $(SRCDIR)/demo.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c 
	$(CC) demo.o init.o -o demo.exe $(LDFLAGS)

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
	$(CC) clientUDP.o -o clientUDP.exe $(LDFLAGS)
	$(CC) serverUDP.o -o serverUDP.exe $(LDFLAGS)

mapC:
	@echo "Building MapCreator"
	$(CC) $(CFLAGS) $(SRCDIR)/mapCreator.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c 
	$(CC) mapCreator.o init.o -o mapCreator.exe $(LDFLAGS)

clean:
	rm *.exe
	rm *.o
