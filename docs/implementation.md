# PET Game Implementation Details

## Code Structure

The game is implemented in C and consists of the following main components:

### Main Source Files
- `src/main.c`: Core game implementation
- `src/game/game.h`: Game-specific declarations and constants

### Memory Layout
- Screen RAM: `0x8000`
- Screen dimensions: 40x20 characters
- Timer location: `0xe840`

### Zero Page Usage
- Text temp address pointer: `0x68`
- Destination pointer: `0x6A`
- Map pointer: `0x6C`
- Screen destination: `0x6E`

### Hardware Registers
- Interrupt flag: `0xe813`
- Key row: `0xe810`
- Key read: `0xe812`

## Core Game Components

### Game State Management
- Tick counter for timing
- Game loop with vsync synchronization
- Input handling system
- Screen buffer management

### Game Objects

#### Enemies
```c
struct Enemy {
    int x, y;           // Position relative to screen
    unsigned char active;        // Whether enemy is alive
    unsigned char sprite;        // Character to display
    unsigned char move_type;     // Type of movement pattern
    unsigned char move_counter;  // For timing movement
};
```

#### Bullets
```c
struct Bullet {
    int x, y;
    unsigned char active;
};
```

### Core Functions

#### Screen Management
- `init_screen()`: Initialize screen state
- `txt_cls()`: Clear screen
- `txt_move_to()`: Position cursor
- `txt_print_string()`: Output text
- `txt_clear_buffer()`: Clear text buffer
- `draw_screen()`: Render game state

#### Game Logic
- `init_game()`: Game initialization
- `handle_input()`: Process user input
- `update_bullets()`: Bullet movement and collision
- `update_enemies()`: Enemy AI and movement
- `draw_ship()`: Player ship rendering

#### Utility Functions
- `wait_vsync()`: Screen refresh synchronization
- `txt_wait_key()`: Input blocking
- `Key_Read()`: Keyboard input processing

## Technical Details

### Display System
- Character-based display
- Direct memory mapping to screen RAM
- Double buffering for smooth animation
- Vsync-based timing for stable frame rate

### Input System
- Keyboard matrix scanning
- Direct hardware register access
- Debounced input processing

### Game Loop
1. Input processing
2. Game state update
3. Collision detection
4. Screen rendering
5. Vsync wait

### Performance Considerations
- Zero page usage for critical variables
- Direct memory access for screen updates
- Optimized character-based graphics
- Efficient collision detection

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