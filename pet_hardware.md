# Commodore PET 4032 Hardware Specifications

## System Overview

The Commodore PET 4032 is a second-generation PET computer released in 1980. The "40" indicates a 40-column screen, and "32" refers to 32KB of RAM.

## CPU and Memory

### Processor
- MOS Technology 6502
- Clock Speed: 1 MHz
- 8-bit data bus
- 16-bit address bus
- Key Features:
  - Zero page addressing (fast memory access)
  - Limited register set (A, X, Y, SP, PC)
  - Hardware stack limited to 256 bytes
  - No hardware multiplication/division

### Memory Map
```
$0000-$00FF: Zero Page
$0100-$01FF: System Stack
$0200-$03FF: System Variables/Buffers
$0400-$7FFF: BASIC Program Space/User RAM
$8000-$83E7: Screen Memory (40x25 = 1000 bytes)
$8400-$8FFF: Reserved
$9000-$AFFF: I/O Area
$B000-$DFFF: BASIC ROM
$E000-$FFFF: KERNAL ROM
```

## Video System

### Display Specifications
- 40x25 character text display
- Monochrome green phosphor CRT
- Memory-mapped at $8000
- No hardware sprites
- No bitmap graphics mode
- Screen refresh rate: 60Hz (NTSC) or 50Hz (PAL)

### Character Set
- 256 built-in characters
- Upper/lowercase switchable
- PETSCII graphics characters:
  - Block graphics (0xE0-0xEE)
  - Card suit symbols
  - Line drawing characters
  - Business graphics symbols

## Input/Output

### Keyboard
- Full-size QWERTY layout
- Separate numeric keypad
- No function keys
- Key matrix scanning via 6520 PIA
- No hardware key repeat
- Keyboard buffer at $026F-$0278

### I/O Ports
- IEEE-488 (GPIB) parallel port
- 2x Commodore cassette ports
- Memory expansion port
- User port (parallel I/O)

### Peripheral Interface Adapters
1. 6520 PIA #1 ($E810-$E81F)
   - Keyboard scanning
   - IEEE-488 control
2. 6520 PIA #2 ($E820-$E82F)
   - IEEE-488 data
   - Cassette control

## Sound Capabilities

- Single-bit output through CB2 line
- No dedicated sound chip
- Software-generated sound only
- Typical frequencies: 20Hz to 10KHz
- Programming techniques:
  - Bit-banging
  - Timer-based tone generation
  - CPU cycle-counted delays

## Programming Considerations

### Memory Management
- Limited RAM: Must be efficient
- No MMU: Direct memory access
- Stack limitations: Careful with recursion
- Zero page: Critical for performance

### Graphics Programming
- Character-based only
- Screen updates must be timed with refresh
- Double buffering recommended for animation
- PETSCII graphics provide pseudo-graphics

### Timing and Interrupts
- 60/50Hz system IRQ
- Timer accuracy: 1MHz clock
- No hardware sprite collision
- CPU handles all timing
- Cycle counting critical for precise timing

### Performance Tips
1. Use zero page variables for speed
2. Unroll time-critical loops
3. Pre-calculate lookup tables
4. Minimize screen updates
5. Use self-modifying code when necessary

## Hardware Limitations

1. Display
   - No hardware scrolling
   - No sprite support
   - No color support
   - Limited to character mode

2. Memory
   - No bank switching
   - Fixed ROM locations
   - Limited RAM for programs

3. Sound
   - No dedicated sound hardware
   - Limited to simple beeps
   - High CPU overhead for sound

4. Speed
   - 1MHz CPU limiting factor
   - No DMA support
   - All graphics handled by CPU

## Programming Workarounds

### Smooth Scrolling
- Use character buffer technique
- Update during vertical blank
- Limit scroll speed to maintain performance

### Sprite-like Graphics
- Use PETSCII characters
- Implement software sprite system
- Use character combinations for detail

### Sound Generation
- Use timer interrupts
- Implement software envelope
- Balance sound vs. performance

### Animation
- Character-based animation
- Double buffering
- Screen update synchronization
- Efficient update regions 