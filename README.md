# Project Name: Cat Clash - Multiplayer Game

![Cat Clash](cat_clash_banner.png)

## Table of Contents
- [Description](#description)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Description
Cat Crashers is a multiplayer game developed as part of the HI1038 "Projektkurs inom data- och nätverksteknik" course. The game revolves around adorable cats crashing into each other, and it is built using the SDL2 library in the C programming language.

## Features
- Multiplayer functionality: Play with your friends and crash into each other's cats.
- Cute cat characters: Choose from a variety of cute cat characters to crash around the game world.
- Collision physics: Experience realistic collision physics as the cats bump into each other.
- Scoring system: The last cat alive wins.

## Requirements
- C compiler
- SDL2 library
- SDL2_image library
- SDL2_ttf library
- SDL2_net library
- dirent library
- pthreads library

## Installation
1. Clone the repository:

   ```
   git clone https://github.com/Phillezi/Grupp-4-KittenFork.git
   ```

2. Install the necessary libraries:

   ```
   # Example command for Msys2
   pacman -S mingw64/mingw-w64-x86_64-SDL2 mingw64/mingw-w64-x86_64-SDL2_image mingw64/mingw-w64-x86_64-SDL2_ttf mingw64/mingw-w64-x86_64-SDL2_mixer mingw64/mingw-w64-x86_64- SDL2_net
   ```

3. Build the game:

   ```
   # Windows
   mingw32-make.exe all
   ```

## Usage
1. Run the game:

   ```
   ./main
   ```

2. Use the arrow keys or W A S D + space to control your cat and crash into other cats.
3. Enjoy the multiplayer gaming experience with your friends!
