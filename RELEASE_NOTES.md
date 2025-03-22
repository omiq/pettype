# PET-Type v1.0 Release Notes

This release represents a complete working implementation matching the original 2021 assembly version.

## Features

- Complete scrolling map implementation (400×24 characters)
- Player ship with PETSCII character graphics
- Collision detection (can be disabled for testing)
- Smooth scrolling background
- Support for both regular keys and numeric keypad
- Double-buffered screen updates
- Vertical blank synchronization

## Technical Details

- Target Platform: Commodore PET 4032
- Memory Requirements: 32KB RAM
- Screen Mode: 40×24 text mode
- Map Size: 9600 bytes (400×24 characters)

## Controls

Regular Keys:
- Up: W
- Down: S
- Left: A
- Right: D

Numeric Keypad:
- Up: 8
- Down: 2
- Left: 4
- Right: 6

## Files

- game.prg: Main game executable for PET emulator

## Running the Game

1. Load game.prg in a PET 4032 emulator
2. Type RUN to start the game
3. Use keyboard controls to navigate
4. Avoid collisions with the environment

## Known Issues

- None (matches original assembly implementation)

## Debug Features

- Collision detection can be disabled for testing 