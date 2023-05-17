# Makefile for Windows
SRCDIR=./source
CC=gcc
CFLAGS = -g -c 
LDFLAGS = -Wall -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_net -lSDL2_ttf -pthread -liphlpapi -lws2_32 #-mwindows -lm

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
	$(CC) main.o init.o text.o player.o menu.o levelEditor.o ioHandler.o newClient.o getDefaultGateway.o -o main.exe $(LDFLAGS)
	@echo "Building Server"
	$(CC) $(CFLAGS) $(SRCDIR)/betterServer.c 
	$(CC) betterServer.o init.o text.o ioHandler.o player.o -o betterServer.exe $(LDFLAGS)

all2:
	@echo "Building Application"
	$(CC) $(CFLAGS) ./src/app.c
	$(CC) $(CFLAGS) ./src/window.c
	$(CC) $(CFLAGS) ./src/network.c
	$(CC) $(CFLAGS) ./src/game.c
	$(CC) $(CFLAGS) ./src/render.c
	$(CC) $(CFLAGS) ./src/input.c
	$(CC) $(CFLAGS) ./src/client.c
	$(CC) $(CFLAGS) ./src/menu.c
	$(CC) $(CFLAGS) ./src/text.c
	$(CC) $(CFLAGS) ./src/player.c
	$(CC) $(CFLAGS) ./src/tile.c
	$(CC) app.o window.o network.o game.o render.o input.o client.o menu.o text.o player.o tile.o -o application.exe $(LDFLAGS)
	

debug:
	@echo "Building main debug"
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c
	$(CC) $(CFLAGS) $(SRCDIR)/text.c
	$(CC) $(CFLAGS) $(SRCDIR)/player.c  
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c    
	$(CC) $(CFLAGS) $(SRCDIR)/levelEditor.c
	$(CC) $(CFLAGS) $(SRCDIR)/ioHandler.c
	$(CC) $(CFLAGS) $(SRCDIR)/newClient.c
	$(CC) $(CFLAGS) $(SRCDIR)/getDefaultGateway.c
	$(CC) main.o init.o text.o player.o menu.o levelEditor.o ioHandler.o newClient.o getDefaultGateway.o -o main.exe $(LDFLAGS)
	@echo "Building Server"
	$(CC) $(CFLAGS) $(SRCDIR)/betterServer.c 
	$(CC) betterServer.o init.o text.o ioHandler.o player.o -o betterServer.exe -g3 $(LDFLAGS)

launcher:
	@echo "Building GameLauncher"
	$(CC) $(CFLAGS) $(SRCDIR)/gameLauncher.c
	$(CC) gameLauncher.o -o launcher.exe 

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

netThread:
	@echo "Building Experimental networking"
	$(CC) $(CFLAGS) $(SRCDIR)/multiThreadedServer.c 
	$(CC) $(CFLAGS) $(SRCDIR)/init.c 
	$(CC) $(CFLAGS) $(SRCDIR)/text.c 
	$(CC) multiThreadedServer.o init.o text.o -o MTserver.exe $(LDFLAGS)

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
