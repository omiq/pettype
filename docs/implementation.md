# PET-Type Implementation Documentation (v1.0)

This document details the implementation of PET-Type v1.0, which matches the original 2021 assembly version.

## Core Components

### 1. Screen and Memory Management

The game uses a double-buffering system for smooth graphics:

- Screen Buffer: Located at `$8000`
- Screen Size: 40×24 characters (visible area)
- Map Size: 400×24 characters (scrollable area)
- Double buffering:
  - Working buffer: `screen_buffer`
  - Display buffer: `$8000`

### 2. Map System

The game world is stored in a large character map:

- Total map size: 9600 bytes (400×24)
- Each row is 400 bytes wide
- Scrolling implemented via offset (sx) when copying from map to screen
- Map data contains PETSCII characters forming the game environment
- Map wraps horizontally for infinite scrolling

### 3. Sprite System

Player ship sprite characteristics:

- Size: 3 rows × 5 columns of PETSCII characters
- Collision detection:
  - Checks character values at sprite position
  - Can be disabled with `collision_off` flag for debugging
- Sprite drawing uses direct memory writes to screen buffer
- Multi-character composition creates detailed sprite appearance

### 4. Game Loop

The main game loop follows this sequence:

```c
1. Wait for vertical blank start (Screen_WaitVblStart)
2. Update game state:
   - Process keyboard input
   - Update player position
   - Handle scrolling
3. Draw frame:
   - Copy map section to screen buffer
   - Draw ship sprite
   - Copy screen buffer to display
4. Wait for vertical blank end (Screen_WaitVblEnd)
```

### 5. Input Handling

The game supports two key mappings:

Regular Keys:
```
Up    = $0103
Down  = $0105
Left  = $0104
Right = $0204
```

Numeric Keypad:
```
Up    = $4003
Down  = $4005
Left  = $4004
Right = $8004
```

### 6. Screen Drawing

The `draw_screen()` function handles map rendering:

```c
draw_screen():
1. Calculate map offset based on scroll position (sx)
2. For each screen row (0-23):
   - Calculate row offset in map (row × 400 + sx)
   - Copy 40 bytes from map to screen buffer
   - Advance to next row
```

### 7. Collision Detection

Collision handling in `draw_ship()`:

```c
draw_ship():
1. Calculate ship position in screen buffer
2. If collision detection enabled:
   - Check characters at ship position
   - If non-space character found, trigger collision
3. Draw ship PETSCII characters (3×5 pattern)
```

### 8. Game States

Game state is managed through global flags:

- `ALIVE`: Controls game loop continuation
- `WON_GAME`: Indicates victory condition

Screen states:
- Title screen
- Game play
- Win/lose screen

### 9. Timing and Synchronization

The game uses several timing mechanisms:

- Vertical blank synchronization for screen updates
- Tick counter for animation timing
- Screen updates synchronized to avoid tearing
- Frame-based movement and scrolling

### 10. Memory Layout

Key memory locations:

```
$0401: BASIC stub
$0410: Main code
$8000: Screen memory
screen_buffer: Working buffer for screen updates
map: 9600 byte map data
```

## Debug Features

- `collision_off` flag: Disables collision detection for testing
- Memory-mapped screen allows direct inspection of display buffer
- Separate screen buffer enables debugging of rendering process

## Known Limitations

- Fixed map size (400×24)
- Single sprite implementation
- No sound effects
- Limited game states (alive/dead/won)

## Future Improvements

Potential areas for enhancement:

1. Sound effects and music
2. Multiple sprites/enemies
3. Power-ups and scoring
4. Enhanced collision detection
5. Additional game states
6. Level progression 