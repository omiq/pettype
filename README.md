# PET-Type

A Commodore PET game implemented in C, matching the original 2021 assembly version.

## Documentation

- [Implementation Details](docs/implementation.md) - Detailed technical documentation of the current implementation
- [Hardware Reference](docs/pet_hardware.md) - PET hardware specifics and memory layout
- [Graphics API](docs/graphics_api.md) - Documentation of the graphics/screen handling system
- [Development History](docs/part1.md) - Development process and history

## Building

```bash
make clean && make
```

This will produce `game.prg` which can be run in a PET emulator.

## Version History

- v1.0-assembly-match: Complete working implementation matching original 2021 assembly version

## Requirements

- Commodore PET 4032 (or emulator)
- Minimum 32KB RAM
- cc65 compiler suite

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