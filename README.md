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
   ### Windows: Using the Msys2 terminal
   ``` MSYS2 MSYS
   pacman -S mingw64/mingw-w64-x86_64-SDL2 mingw64/mingw-w64-x86_64-SDL2_image mingw64/mingw-w64-x86_64-SDL2_ttf mingw64/mingw-w64-x86_64-SDL2_mixer mingw64/mingw-w64-x86_64- SDL2_net
   ```
   ### Linux, Ubuntu:
   ``` bash
   sudo apt install -y libsdl2-2.0-0 libsdl2-image-2.0-0 libsdl2-mixer-2.0-0 libsdl2-ttf-2.0-0 libsdl2-net-2.0-0 libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev libsdl2-mixer-dev
   ```
   ### MacOS:
   ```
   #Lägg in MacOS kommando här
   ```

3. Build the game:
   ### Windows:
   ```
   mingw32-make.exe all
   ```
   ### Linux:
   ```
   make all
   ```
   ### MacOS:
   ```
   make all
   ```

## Usage
1. Run the game:

   ```
   ./main
   ```

2. Use the arrow keys or W A S D + space to control your cat and crash into other cats.
3. Enjoy the multiplayer gaming experience with your friends!
