# PET Game Implementation Guide

## Project Overview

This project implements a game for the Commodore PET, written in C using direct hardware access. The game features character-based graphics, keyboard input, and smooth animation through double buffering.

## Technical Specifications

- Screen Resolution: 40x20 characters
- Screen Memory: Located at `0x8000`
- Input: PET Keyboard via hardware registers
- Graphics: Character-based using PETSCII

## Project Structure

```
.
├── src/
│   ├── main.c         # Core game implementation
│   ├── game/
│   │   └── game.h     # Game-specific declarations
└── docs/
    ├── implementation.md
    ├── graphics_api.md
    └── pet_hardware.md
```

## Core Components

### Header Files

```c
#include <stdio.h>      // Standard I/O operations
#include <string.h>     // String manipulation
#include <peekpoke.h>   // Direct memory access
#include <conio.h>      // Console I/O
```

### Key Data Structures

```c
// Enemy entity
struct Enemy {
    int x, y;                    // Position
    unsigned char active;        // Status
    unsigned char sprite;        // Display character
    unsigned char move_type;     // Movement pattern
    unsigned char move_counter;  // Animation timing
};

// Projectile
struct Bullet {
    int x, y;                    // Position
    unsigned char active;        // Status
};
```

### Memory Layout

Important memory locations:
```
0x8000: Screen Memory (800 bytes)
0xe810: Key Row Register
0xe812: Key Read Register
0xe813: Interrupt Flag
0xe840: Timer Location
```

### Zero Page Usage

Critical variables:
```
0x68: Text temp address
0x6A: Destination pointer
0x6C: Map pointer
0x6E: Screen destination
```

## Core Functions

### Screen Management
- `init_screen()`: Initialize display
- `txt_cls()`: Clear screen
- `txt_move_to()`: Position cursor
- `txt_print_string()`: Output text
- `draw_screen()`: Update display

### Game Logic
- `init_game()`: Setup game state
- `handle_input()`: Process keyboard
- `update_bullets()`: Projectile logic
- `update_enemies()`: Enemy AI
- `draw_ship()`: Player graphics

### Utility Functions
- `wait_vsync()`: Display sync
- `txt_wait_key()`: Input handling
- `Key_Read()`: Keyboard scanning

## Implementation Details

### Display System
1. Double buffering for smooth animation
2. Direct memory writes to screen RAM
3. Character-based sprite system
4. Vsync-synchronized updates

### Input Processing
1. Hardware register polling
2. Keyboard matrix scanning
3. Input debouncing
4. Multiple key support

### Game Loop
1. Process input
2. Update game state
3. Handle collisions
4. Render screen
5. Sync display

### Performance Optimization
1. Zero page variable usage
2. Efficient memory access
3. Minimal screen updates
4. Synchronized timing

## Technical Limitations

### Display
- Fixed character resolution
- Monochrome output
- Character-cell movement
- PETSCII graphics only

### Memory
- Fixed screen location
- Limited zero page space
- No hardware sprites
- Character-based graphics

### Timing
- Vsync-dependent updates
- Input polling overhead
- Animation constraints
- Character movement limits 