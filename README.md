# Grupp 4 HI1038, Cat Clash - Multiplayer Game

![Cat Clash](cat_clash_banner.png)

## Table of Contents
- [Description](#description)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)

## Description
Cat Clash is a multiplayer game developed as part of the HI1038 "Projektkurs inom data- och nätverksteknik" course. The game revolves around adorable cats crashing into each other, and it is built using the SDL2 library in the C programming language.

## Features
- Multiplayer functionality: Play with your friends and crash into each other's cats.
- Cute cat characters: Choose from a variety of cute cat characters to crash around the game world.
- Create your own maps!: Using the built in level-editor you can create fun and unique maps to play with your friends.
- Competitive gameplay: The last cat alive wins.

## Requirements
- C compiler
- SDL2 library
- SDL2_image library
- SDL2_ttf library
- SDL2_net library
- SDL2_mixer library (can be disabled by removing "#include <SDL2/SDL_mixer.h>" from "include/definitions.h")
- dirent library
- pthread library

## Installation
1. Clone the repository:

   ```bash
   git clone https://github.com/Phillezi/Grupp-4-KittenFork.git
   ```

2. Install the necessary libraries:
   ### Windows: Using the Msys2 terminal.
   ``` MSYS2 MSYS
   pacman -S mingw64/mingw-w64-x86_64-SDL2 mingw64/mingw-w64-x86_64-SDL2_image mingw64/mingw-w64-x86_64-SDL2_ttf mingw64/mingw-w64-x86_64-SDL2_mixer mingw64/mingw-w64-x86_64- SDL2_net
   ```
   ### Linux, Ubuntu: Using apt package manager.
   ``` bash
   sudo apt install -y libsdl2-2.0-0 libsdl2-image-2.0-0 libsdl2-mixer-2.0-0 libsdl2-ttf-2.0-0 libsdl2-net-2.0-0 libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev libsdl2-mixer-dev
   ```
   ### MacOS: On mac with homebrew installed please use the following command in terminal to install the libraries.
   ```terminal
   brew install sdl2 sdl2_mixer sdl2_image sdl2_ttf sdl2_net
   ```

3. Build the game:
   ### Windows:
   ```powershell
   mingw32-make.exe all
   ```
   ### Linux:
   ```bash
   make all
   ```
   ### MacOS:
   ```terminal
   make all
   ```

## Usage
## 0. Toggle settings in the config file ("/resources/config.txt").
   Here you can toggle fps, vsync, resolution, music and sound volume and enable/disable multithreading.
## 1. Run the game:

   ```bash
   ./main
   ```
## 2. Run the server:

   ```bash
   ./betterServer
   ```
   Then select a local map to host using the GUI of the server.
## 3. Join the hosted server;
   ### Take note of the IP-Address of the computer that is hosting the server:
   #### Windows:
   ##### To play in LAN mode
   ```powershell
   ipconfig
   ```
   Then look for "IPv4 Address. . . . . . . . . . . : [your local ip here]"
   
   ##### To play in WAN mode
   If you want to play over the internet the process isnt quite as simple.
   You will need to open your routers settings and port forward the port "1234" over both UDP and TCP to your local address of the computer hosting the network with the same port: "1234".
   NOTE: This is not ideal as it opens up your network to the entire internet and makes you more vulnerable. 
   DO THIS AT YOUR OWN RISK.
   
   ### Join using the GUI
   #### Windows specific features:
   ##### Scan local network:
   On windows you have the ability to scan for games on your local network, NOTE: This only works for Class C networks and creates a lot of unnecessary network traffic.
   ##### Scan from list:
   You also have the ability to scan for servers from a list containing saved ips, this list is located in "resources/network/saved_ips.txt".
   You can add ips to the list as plain text with one ip per line or separating them with a space, the list can be changed during runtime.
   #### Type in ip:
   In the "Join Server" menu, press search and type in the ip-address of the desired server to join. 
   
## 4. Play the game.
   ### Lobby:
   When you have joined the server you will be greeted by a lobby, this lobby shows the other joined players and their cats.
   Here you can wait for all your friends to join and then press space to go in to the game level.
   ### Controls:
   You can move your cat using W A S D or the arrow keys, and space to charge.
## 5. Enjoy the multiplayer gaming experience with your friends!
