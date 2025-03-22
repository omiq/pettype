# PET Horizontal Shooter

A horizontal scrolling shooter game for the Commodore PET 4032, inspired by classic arcade games.
Written in C using the cc65 compiler suite.

## Requirements

- Commodore PET 4032 (or emulator)
- Minimum 32KB RAM
- cc65 compiler suite

## Building

1. Install cc65 compiler suite
2. Run `make` to build the game
3. The output file `game.prg` can be loaded on a PET or emulator

## Controls

- W: Move Up
- S: Move Down
- A: Move Left
- D: Move Right
- SPACE: Fire
- F: Charge Shot

## Technical Details

- Uses PET's 40x25 text screen at $8000
- PETSCII block graphics for sprites
- Double buffering for smooth scrolling
- Keyboard matrix scanning for input 