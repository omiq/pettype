
![image.png](https://files.peakd.com/file/peakd-hive/makerhacks/23vrdXsWTbezKhx5N5hv38venYXbcqJy4rUqyk6U9cZ9p5oYtZyZhmm9hhZ99c28CRn6m.png)


Back in 2021, I started working on a game for the Commodore PET. The original version was written in pure 6502 assembly, via TRSE - a challenging but rewarding experience. 

Life *happened*, and the project sat dormant until this weekend when I decided to dust it off and give it new life by converting it to C using the CC65 compiler.


![output.gif](https://files.peakd.com/file/peakd-hive/makerhacks/23swXV961FpHNt2hAKWFTdq3hmPNScrJYkD2XyC39iKJyDDD7BZsoAyAkAYvkPbTNxEuZ.gif)


## Revisiting a 2021 Project : From Assembly to C

My project draws inspiration from Irem's iconic 1987 arcade game R-Type, though obviously with significant adaptations for the PET's limited hardware. 

While I can't match the arcade's beautiful 16-bit graphics or complex sprite system, I can capture some of the core elements that made R-Type special:

1. **Horizontal Scrolling**
   - R-Type had smooth pixel-perfect scrolling, I'm implementing character-by-character scrolling
   - Using PETSCII characters creatively to suggest a sci-fi environment
   - The scrolling creates the same sense of progression and discovery

2. **Core Mechanics**
   - Player ship that can shoot
   - Enemy patterns and movement
   - Basic collision detection
   - The fundamental "dodge and shoot" gameplay loop

3. **Level Design Philosophy**
   - My planned tile system will allow for R-Type-style level layouts
   - Strategic placement of enemies and obstacles
   - Creating "patterns" players need to learn and master

### Key Differences


   - No Force pod (R-Type's iconic power-up system)
   - Simpler enemy patterns due to character-grid movement
   - No charge beam system (though I could implement a basic version)

**Technical Constraints**
   - 1MHz CPU vs arcade hardware
   - Character-based collision vs pixel-perfect
   - Limited sound capabilities
   - No hardware sprites or scrolling

## Capturing the Spirit

I can't recreate R-Type's technical achievements, but I can capture elements of what made it engaging:
- Memorizable enemy patterns
- Skill-based gameplay
- Strategic positioning
- Progressive difficulty
- The satisfaction of mastering a level and the motivation to see what comes next.

My goal isn't to clone R-Type (even if that was possible), but to create an engaging shooter for the PET that pays homage to it.

## The Conversion Journey: Assembly to C

Converting from assembly to C using CC65 was an interesting challenge. My original assembly code had direct hardware access for screen manipulation and keyboard input. I needed to preserve what was good while making it more readable and maintainable in C.

### Key Challenges I Faced

1. **Memory Management**
   - My original assembly code used zero-page locations very specifically
   - Had to carefully map my C variables to maintain the same memory efficiency
   - Preserved critical zero page usage for: text pointer (0x68), destination (0x6A), map pointer (0x6C), and screen destination (0x6E)

2. **Screen Handling**
   - Assembly version had direct PETSCII character manipulation
   - Converted to a more structured approach while keeping direct screen memory access at 0x8000
   - Experimented with double buffering for smoother screen updates / animation
   - Maintained the 40x20 character display constraints

3. **Hardware Register Access**
   - Had to map key hardware registers in C:
     - Interrupt flag (0xe813)
     - Timer location (0xe840)

### Solutions and Improvements

So why use C if the assembly obviously worked?

Using a higher level language, with the option to *drop down* to low-level assembly, gives us the quality of life and maintainability benefits.

1. **Structured Data Types**
   - Introduced proper C structures for game entities:
   ```c
   struct Enemy {
       int x, y;
       unsigned char active;
       unsigned char sprite;
       unsigned char move_type;
       unsigned char move_counter;
   };
   ```

2. **Function Organization**
   - Broke down my monolithic assembly code into logical C functions
   - Created clear separation between screen management, game logic, and input handling
   - Maintained direct hardware access where needed for performance

## New Features

The C conversion allowed me to add several features that were challenging in assembly more easily:

1. **Enhanced Enemy System**
   - Multiple enemy types with different movement patterns
   - Proper enemy state management
   - Movement counter for timing animations

2. **Improved Bullet System**
   - More sophisticated projectile handling
   - Better collision detection
   - Active state management

3. **Better Screen Management**
   - The option of double buffering implementation
   - More efficient screen updates
   - Better vsync handling

## Optimizations

While C code is generally less efficient than hand-crafted assembly, I've implemented several optimizations to maintain performance:

1. **Memory Access**
   - Strategic use of zero page for critical variables
   - Direct memory writes for screen updates
   - Efficient buffer management

2. **Display Updates**
   - Synchronized with vertical blank
   - Character-based sprite system
   - Optimized screen buffer copying



## Current State

The game now has a solid foundation in C while maintaining much of the performance characteristics of my original assembly version. The codebase is more maintainable and easier to extend, while still taking into account the hardware limitations of the PET.

## Planned Improvements

### Tile-Based Map System
Currently, the game uses a hard-coded array for level data. My next major update will implement a proper tile-based map system:

1. **Map Structure**
   ```c
   struct MapTile {
       unsigned char type;      // Tile type (wall, floor, hazard, etc.)
       unsigned char properties; // Collision, damage, etc.
   };
   ```

2. **Level Format**
   - Simple text-based level files for easy editing
   - Tile properties defined in a separate configuration
   - Support for multiple levels
   - Memory-efficient tile storage

3. **Map Loading**
   - Load level data from memory
   - Dynamic tile property lookup
   - Efficient scrolling and rendering
   - Tile-based collision detection

This will make it much easier for me to create and modify levels, as well as add different types of environments to the game.

The trick will be implementing this without causing any added slow-downs in gameplay.

### Other Planned Features
1. Simple sound effects using the PET's single-bit audio output (basic beeps for shooting, collisions, etc.)
2. More enemy types and patterns
3. Power-up system
4. Enhanced collision detection
5. Level progression

*Note about sound: The stock Commodore PET only has a single-bit output through the CB2 line, which means I'm limited to basic beeps and buzzes. While this is quite limiting compared to later Commodore machines like the C64 with its SID chip, I can still use simple sound effects to enhance the gameplay experience. These will need to be carefully timed using the CPU, as there's no dedicated sound hardware to offload to.*

## Technical Considerations

### Memory Budget
- Screen Memory: 800 bytes (40x20)
- Tile Data: ~256 bytes for tile properties
- Level Data: Variable based on map size
- Game Logic: Remaining available RAM

### Performance Impact
- Need to optimize tile lookups
- Efficient map scrolling algorithms
- Careful memory access patterns
- Balance between features and speed

The tile system will need careful optimization to maintain a smooth gameplay experience while adding the flexibility of proper level design.

## To Sum Up

Converting from assembly to C has opened up new possibilities for extending my game while teaching me valuable lessons about both languages. 

The planned tile system will significantly improve my current hard-coded approach, making the game more engaging and easier to expand.

This project continues to be a fun exercise in working within the constraints of truly vintage hardware while attempting/implementing modern game design patterns! 