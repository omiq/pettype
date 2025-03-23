# PET Hardware Implementation Details

## Hardware Features Used

### Display System
- 40x20 character text display
- Memory-mapped at `0x8000`
- Monochrome display
- PETSCII character set for graphics

### Memory Map
Important memory locations used in implementation:
```
$8000: Screen Memory (40x20 = 800 bytes)
$e810: Key Row Register
$e812: Key Read Register
$e813: Interrupt Flag Register
$e840: Timer Location
```

### Zero Page Usage
Critical zero page locations used:
```
$68: Text temp address pointer
$6A: Destination pointer
$6C: Map pointer
$6E: Screen destination
```

### Input System
- Keyboard matrix scanning via hardware registers
- Key row and read registers at `0xe810` and `0xe812`
- Debounced input processing

### Timing
- Vertical sync for screen updates
- Timer at `0xe840`
- Interrupt handling via `0xe813`

## Implementation Considerations

### Display Programming
- Character-based graphics only
- Direct memory writes to screen RAM
- Double buffering for smooth animation
- Screen updates synchronized with vertical blank

### Memory Management
- Efficient use of zero page for pointers
- Direct screen memory access
- Careful management of screen buffer

### Input Processing
- Hardware register polling
- Matrix keyboard scanning
- Efficient input debouncing

### Performance Optimization
1. Zero page usage for critical variables
2. Direct memory access patterns
3. Efficient screen update routines
4. Synchronized display updates

## Technical Limitations

### Display Constraints
- Fixed 40x20 character resolution
- Single color (monochrome) output
- Character-cell movement only
- PETSCII character set limitations

### Memory Constraints
- Fixed screen memory location
- Limited zero page space
- No hardware sprites
- No bitmap graphics mode

### Timing Considerations
- Vertical sync timing critical
- Input polling overhead
- Screen update synchronization
- Character-based animation limits 