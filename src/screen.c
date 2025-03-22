#include <peekpoke.h>
#include <string.h>

#define SCREEN_RAM 0x8000
#define SCREEN_COLS 40
#define SCREEN_ROWS 25

// PETSCII block characters for 4x4 pixel mapping
const unsigned char petscii_blocks[16] = {
    0x20, // Empty
    0xE0, // ▘
    0xE1, // ▝
    0xE2, // ▀
    0xE3, // ▖
    0xE4, // ▌
    0xE5, // ▞
    0xE6, // ▛
    0xE7, // ▗
    0xE8, // ▚
    0xE9, // ▐
    0xEA, // ▜
    0xEB, // ▄
    0xEC, // ▙
    0xED, // ▟
    0xEE  // █
};

// Double buffer for smooth scrolling
unsigned char screen_buffer[SCREEN_COLS * SCREEN_ROWS];
unsigned char scroll_offset;

void init_screen(void) {
    memset(screen_buffer, 0x20, sizeof(screen_buffer));
    scroll_offset = 0;
}

// Convert 4x4 bitmap to PETSCII character
unsigned char bitmap_to_petscii(const unsigned char bitmap[4][4]) {
    unsigned char index = 0;
    unsigned char bit = 1;
    unsigned char y, x;
    
    // Convert 4x4 bitmap to 4-bit index
    for(y = 0; y < 2; y++) {
        for(x = 0; x < 2; x++) {
            if(bitmap[y*2][x*2]) {
                index |= bit;
            }
            bit <<= 1;
        }
    }
    
    return petscii_blocks[index];
}

void scroll_screen(void) {
    unsigned char y, x, screen_x;
    scroll_offset = (scroll_offset + 1) % SCREEN_COLS;
    
    // Copy buffer to screen with offset
    for(y = 0; y < SCREEN_ROWS; y++) {
        for(x = 0; x < SCREEN_COLS; x++) {
            screen_x = (x + scroll_offset) % SCREEN_COLS;
            POKE(SCREEN_RAM + y * SCREEN_COLS + x,
                 screen_buffer[y * SCREEN_COLS + screen_x]);
        }
    }
}

void plot_sprite(unsigned char x, unsigned char y, const unsigned char* sprite_data,
                unsigned char width, unsigned char height) {
    unsigned char sy, sx, buffer_x, buffer_y;
    for(sy = 0; sy < height; sy++) {
        for(sx = 0; sx < width; sx++) {
            buffer_x = (x + sx) % SCREEN_COLS;
            buffer_y = y + sy;
            if(buffer_y < SCREEN_ROWS) {
                screen_buffer[buffer_y * SCREEN_COLS + buffer_x] =
                    sprite_data[sy * width + sx];
            }
        }
    }
} 