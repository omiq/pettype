# Graphics API Documentation

## Overview

The graphics system is character-based, utilizing the PET's built-in character display capabilities. The screen is memory-mapped at `0x8000` with a resolution of 40x20 characters.

## Core Graphics Functions

### Screen Management

#### `void init_screen(void)`
Initializes the screen system and sets up initial display state.

#### `void txt_cls(void)`
Clears the entire screen to blank characters.

#### `void txt_move_to(void)`
Positions the cursor for subsequent text output.

#### `void txt_print_string(void)`
Outputs a string at the current cursor position.

#### `void txt_clear_buffer(void)`
Clears the text buffer used for double buffering.

#### `void draw_screen(void)`
Main rendering function that updates the entire display.

### Sprite Management

#### `void draw_ship(void)`
Renders the player's ship using character-based graphics.

### Synchronization

#### `void wait_vsync(void)`
Waits for vertical sync to prevent screen tearing.

## Memory Layout

### Screen Memory
- Base Address: `0x8000`
- Screen Width: 40 characters
- Screen Height: 20 characters
- Total Size: 800 bytes

### Character Properties
- Standard PETSCII character set
- Single color (monochrome display)
- Character-based sprites and graphics

## Display Techniques

### Double Buffering
The system uses a double buffering approach for smooth animation:
1. Draw to off-screen buffer
2. Wait for vsync
3. Copy buffer to screen memory

### Character-Based Graphics
- Sprites composed of multiple characters
- Movement in character-cell increments
- Collision detection based on character values

### Screen Updates
- Direct memory writes for efficiency
- Synchronized with vertical blank
- Optimized for character-based operations

## Performance Considerations

### Optimization Techniques
1. Direct memory access for screen updates
2. Zero page usage for critical pointers
3. Minimal screen updates when possible
4. Vsync timing for smooth animation

### Limitations
- Character-cell resolution only
- No hardware sprites
- Single color display
- Limited to PETSCII character set

## Usage Examples

### Basic Screen Output
```c
txt_cls();              // Clear screen
txt_move_to();         // Position cursor
txt_print_string();    // Output text
```

### Animation Loop
```c
wait_vsync();          // Wait for vertical blank
draw_screen();         // Update display
draw_ship();          // Draw player sprite
```

## Constants

```c
#define SCREEN_RAM  0x8000   // Screen memory location
#define SCREEN_COLS 40       // Screen width in characters
#define SCREEN_ROWS 25       // Screen height in characters
```

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