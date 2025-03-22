# PET Graphics Engine API Documentation

## Overview
The PET Graphics Engine provides a reusable foundation for creating character-based games on the Commodore PET. It handles screen management, sprite plotting, and PETSCII character conversion.

## Core Components

### Screen Management
```c
void init_graphics(void);
```
Initializes the graphics system:
- Clears the screen buffer
- Resets scroll offset
- Disables cursor
- Must be called before any other graphics functions

```c
void clear_screen(void);
```
Clears the entire screen buffer:
- Fills with space characters (0x20)
- Does not affect screen RAM until update_screen() is called

```c
void wait_vsync(void);
```
Synchronizes with vertical blank:
- Prevents screen tearing
- Call before updating screen contents
- Currently uses delay loop, will be enhanced with true vsync

### Sprite Operations
```c
void plot_sprite(
    unsigned char x,          // X position (0-39)
    unsigned char y,          // Y position (0-24)
    const unsigned char* data,// Sprite character data
    unsigned char width,      // Sprite width in characters
    unsigned char height      // Sprite height in characters
);
```
Plots a sprite to the screen buffer:
- Handles screen edge wrapping
- Supports variable-sized sprites
- Data array must contain width * height characters
- Does not perform collision detection

### PETSCII Graphics
```c
void bitmap_to_petscii(
    const unsigned char bitmap[4][4], // 4x4 bitmap input
    unsigned char* result             // Resulting PETSCII character
);
```
Converts 4x4 bitmap to PETSCII character:
- Input: 4x4 array where non-zero = pixel set
- Output: Best matching PETSCII block character
- Used for converting bitmap graphics to characters

### Screen Updates
```c
void scroll_screen(void);
```
Performs horizontal scrolling:
- Updates internal scroll offset
- Must be followed by update_screen()
- Wraps content at screen edges

```c
void update_screen(void);
```
Copies buffer to screen RAM:
- Applies current scroll offset
- Call after all buffer updates complete
- Should be called during/after vsync

## Constants

```c
#define SCREEN_RAM  0x8000   // Screen memory location
#define SCREEN_COLS 40       // Screen width in characters
#define SCREEN_ROWS 25       // Screen height in characters
```

## Usage Example

```c
// Initialize graphics
init_graphics();

// Game loop
while(1) {
    wait_vsync();           // Wait for vertical blank
    
    // Update game state
    plot_sprite(x, y, sprite_data, width, height);
    scroll_screen();        // Scroll background
    
    update_screen();        // Copy to screen RAM
}
```

## Performance Considerations

1. Buffer Updates
   - Group multiple sprite plots before update_screen()
   - Update only changed portions when possible
   - Avoid unnecessary screen clears

2. Timing
   - Always call wait_vsync() before screen updates
   - Complete all drawing before update_screen()
   - Keep sprite sizes minimal for faster plotting

3. Memory
   - Screen buffer uses 1000 bytes (40x25)
   - Sprite data stored in ROM when possible
   - Reuse sprite data for similar objects

## Limitations

1. Display
   - 40x25 character resolution
   - No hardware sprites
   - No color support
   - Character-based graphics only

2. Performance
   - Software scrolling only
   - No hardware acceleration
   - Limited by 1MHz CPU speed

## Future Enhancements

1. Planned Features
   - True vsync detection
   - Sprite collision detection
   - Background tile system
   - Animation support

2. Optimization Opportunities
   - Assembly language critical paths
   - Dirty rectangle updates
   - Sprite clipping optimization 