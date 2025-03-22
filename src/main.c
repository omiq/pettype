#include <stdio.h>
#include <string.h>
#include <peekpoke.h>
#include <conio.h>
#define collision_off 0
// Function declarations
void init_game(void);
void init_screen(void);
void txt_DefineScreen(void);
void txt_cls(void);
void txt_move_to(void);
void txt_print_string(void);
void txt_clear_buffer(void);
void txt_wait_key(void);
void start_screen(void);
void end_screen(void);
void Key_Read(void);
void wait_vsync(void);
void draw_screen(void);
void draw_ship(void);
void handle_input(void);

// Memory locations from assembly
#define SCREEN_RAM 0x8000
#define SCREEN_COLS 40
#define SCREEN_ROWS 20
#define TIMER_LOCATION 0xe840

// Zero page addresses (from assembly)
#define txt_temp_address_p 0x68
#define des 0x6A
#define map_p 0x6C
#define sdes 0x6E

// Hardware registers (from assembly)
#define Interrupts_interruptFlag 0xe813
#define Key_KeyRow 0xe810
#define Key_KeyRead 0xe812

// Game state variables (matching assembly)
static unsigned char ticks = 0;
static unsigned char i = 0;
static unsigned char alive = 1;
static unsigned char won_game = 0;
static unsigned char x = 0;
static unsigned char y = 0;
static unsigned char old_x = 0;
static unsigned char old_y = 0;
static unsigned int sx = 0;
static unsigned int sy = 0;

// Text output variables (from assembly)
static unsigned char txt_text_x = 0;
static unsigned char txt_text_y = 0;
static unsigned char txt_CRLF = 1;
static unsigned char txt_next_ch = 0;
static const char* txt_in_str;

// Keyboard state (from assembly)
static unsigned char Key_keys[10];
static unsigned char Key_keysLast[10];
static unsigned char Key_keysPressed[10];
static unsigned char Key_keyState = 0;
static unsigned char Key_temp = 0;

// Screen tables (from assembly)
static unsigned int txt_ytab[SCREEN_ROWS];

// Ship sprite data (from assembly)
static const unsigned char ship[] = {
    124, 251, 236, 98, 123, 32, 251, 236, 252,
    255, 124, 226, 226, 32, 32, 0
};

// Screen buffer (matching assembly)
static unsigned char screen_buffer[SCREEN_COLS * SCREEN_ROWS];

// Fix CHROUT and keyboard handling
#define CHROUT 0xFFD2
#define KEY_BUFFER_COUNT 0x9E
#define KEY_BUFFER_START 0x26F
#define KEY_MATRIX 0xE810
#define KEY_READ 0xE812

// Text output functions from assembly
void txt_cls(void) {
    unsigned char y;
    unsigned int addr;

    // Clear screen using BASIC's CLS routine
    clrscr();

// Pet graphic mode
    printf("%c",142);
    
    // Initialize screen tables
    addr = SCREEN_RAM;
    for(y = 0; y < SCREEN_ROWS; y++) {
        txt_ytab[y] = addr;
        addr += SCREEN_COLS;
    }
}

void txt_move_to(void) {
    gotoxy(txt_text_x, txt_text_y);
}

void txt_print_string(void) {
    cputs(txt_in_str);
    if(txt_CRLF) {
        cputc('\n');
    }
}

void txt_clear_buffer(void) {
    while(kbhit()) {
        cgetc();
    }
}

void txt_wait_key(void) {
    cgetc();
}

// Title screen (from assembly)
void start_screen(void) {
    txt_cls();
    txt_text_x = 10;
    txt_text_y = 10;
    txt_move_to();
    txt_in_str = "pet-type 2025";
    txt_CRLF = 0;
    txt_print_string();
    txt_clear_buffer();
    txt_wait_key();
}

// End screen (from assembly)
void end_screen(void) {
    txt_cls();
    txt_text_x = 10;
    txt_text_y = 10;
    txt_move_to();
    txt_in_str = won_game ? "well done!" : "you lose!";
    txt_CRLF = 0;
    txt_print_string();
    txt_clear_buffer();
    txt_wait_key();
}

void txt_DefineScreen(void) {
    unsigned char txt_y = 0;
    unsigned int txt_temp_address = SCREEN_RAM;

    for(txt_y = 0; txt_y < SCREEN_ROWS; txt_y++) {
        txt_ytab[txt_y] = txt_temp_address;
        txt_temp_address += SCREEN_COLS;
    }
}

void Key_Read(void) {
    unsigned char row;
    Key_keyState = 0;

    for(row = 0; row < 10; row++) {
        POKE(0xE810, row);              // Select keyboard row
        Key_keys[row] = ~PEEK(0xE812);  // Read and invert key state
        Key_keysPressed[row] = (Key_keys[row] ^ Key_keysLast[row]) & Key_keys[row];
        Key_keysLast[row] = Key_keys[row];
        Key_keyState += Key_keys[row];   // Update overall key state
    }
}

void wait_vsync(void) {
    while(!(PEEK(TIMER_LOCATION) & 0x20));
    while(PEEK(TIMER_LOCATION) & 0x20);
}

#define PETSCII_SPACE 32  // Space character in PETSCII
#define PETSCII_CLEAR 0x93  // Clear screen code

// Map data from assembly - 9600 bytes total (24 rows x 400 bytes per row)
static const unsigned char map[9600] = {
    
    32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,160,160,160,126,32,98,123,251,160,160,226,226,126,32,108,108,32,226,236,126,98,160,126,108,127,127,126,126,126,160,97,32,236,236,236,255,123,251,236,123,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,226,126,32,32,32,32,160,252,126,254,225,160,255,108,97,32,98,123,108,32,32,32,32,160,252,126,254,225,160,255,108,97,32,98,123,108,32,225,160,255,108,97,160,252,126,254,32,108,98,32,123,32,32,32,32,98,236,126,32,32,32,254,126,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,226,226,226,32,124,226,251,225,226,32,108,160,236,108,127,127,127,126,108,254,160,126,32,108,127,127,126,108,160,236,32,124,255,255,123,32,123,160,236,254,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,160,160,126,32,251,160,236,32,124,226,126,124,126,32,32,32,124,160,160,126,32,251,160,236,32,124,226,126,124,126,32,251,160,236,32,124,160,160,126,32,226,226,32,226,32,32,32,254,126,32,32,32,98,254,127,255,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,226,226,226,226,251,32,98,160,160,126,108,127,127,126,32,98,160,236,126,108,127,127,127,126,108,160,226,32,123,255,255,255,32,254,97,251,236,254,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,160,97,225,32,32,32,32,32,32,32,32,32,32,32,32,32,32,160,97,225,32,32,32,32,32,32,32,32,32,32,32,225,160,32,97,32,32,108,160,226,251,108,254,160,126,254,98,123,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,251,123,226,126,108,252,108,108,108,127,126,32,98,160,160,226,108,127,127,252,160,98,254,236,32,32,255,255,254,124,108,254,226,32,127,252,160,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,252,32,254,32,32,32,32,32,32,32,32,32,32,32,32,32,32,252,32,254,32,32,32,32,32,32,32,32,32,32,32,225,123,108,97,32,32,254,236,123,225,160,236,32,254,160,226,108,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,226,226,226,126,126,126,126,98,98,160,236,126,32,124,126,127,126,226,226,226,32,108,226,226,255,160,160,160,226,126,98,97,127,126,127,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,226,126,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,226,126,32,32,32,32,32,32,32,32,32,32,32,32,226,226,32,32,32,160,97,108,160,236,108,108,236,108,254,226,126,32,32,108,97,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,251,236,32,32,98,236,108,254,97,123,254,160,160,252,124,123,254,160,98,98,32,123,98,254,98,123,127,126,126,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,251,32,236,32,32,160,123,32,226,108,254,254,123,32,254,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,32,254,97,236,108,236,32,97,252,236,108,160,226,251,252,236,108,160,226,251,252,236,108,160,126,108,126,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,225,254,97,32,32,32,160,123,124,126,32,108,123,254,126,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,98,98,98,254,108,160,97,108,236,255,32,97,226,108,160,127,126,225,226,108,160,127,126,225,226,108,160,127,126,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,225,226,32,32,32,124,252,126,160,160,236,251,160,126,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,226,126,225,126,32,251,236,255,32,126,32,251,160,127,126,32,32,251,160,127,126,32,32,251,160,127,126,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,32,32,32,32,251,236,32,225,160,32,32,251,123,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,226,124,32,32,32,124,226,126,126,32,32,124,226,126,126,32,32,124,226,126,126,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,126,126,32,32,124,97,108,252,124,97,32,123,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,225,236,255,32,32,32,225,160,251,126,32,32,225,160,251,126,32,32,225,160,251,126,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,254,97,225,254,126,252,254,97,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,225,127,160,32,226,32,236,226,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,108,226,123,32,32,124,160,124,252,98,254,97,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,127,32,255,108,32,32,251,123,124,251,236,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,226,32,160,32,32,32,32,98,32,251,123,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,252,123,32,225,126,127,108,97,32,32,32,32,32,32,32,
32,32,32,32,32,98,160,32,32,32,32,32,32,32,32,32,32,32,98,160,236,160,252,123,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,32,32,32,32,32,32,32,32,32,32,32,98,123,32,32,32,32,32,98,160,32,32,32,32,32,32,32,32,32,32,32,98,160,236,160,252,123,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,32,32,32,32,32,32,32,32,32,32,32,98,160,236,160,252,123,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,32,32,32,32,32,32,32,32,32,32,32,98,160,236,160,252,123,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,32,32,32,32,32,32,32,32,32,32,32,98,160,236,160,252,123,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,32,32,32,32,32,32,32,32,32,32,32,98,160,236,160,252,123,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,160,98,32,127,126,254,126,32,32,32,32,32,32,32,
32,32,32,98,160,251,226,160,97,32,32,32,32,32,32,32,108,254,236,127,252,32,226,160,252,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,251,226,160,97,32,32,32,32,32,32,32,108,254,236,251,123,32,32,98,160,251,226,160,97,32,32,32,32,32,32,32,108,254,236,127,252,32,226,160,252,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,251,226,160,97,32,32,32,32,32,32,32,108,254,236,127,252,32,226,160,252,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,251,226,160,97,32,32,32,32,32,32,32,108,254,236,127,252,32,226,160,252,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,251,226,160,97,32,32,32,32,32,32,32,108,254,236,127,252,32,226,160,252,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,98,160,251,226,160,97,32,32,32,32,32,32,32,108,254,236,127,252,32,226,160,252,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,225,236,108,98,123,251,32,32,32,32,32,32,32,32,
32,108,160,160,126,225,32,124,251,160,123,32,32,108,254,254,160,127,127,251,160,32,32,32,251,97,126,32,32,32,254,252,123,32,32,32,32,32,32,32,32,108,160,160,126,225,32,124,251,160,123,32,32,108,254,254,160,127,127,251,160,127,160,160,126,225,32,124,251,160,123,32,32,108,254,254,160,127,127,251,160,32,32,32,251,97,126,32,32,32,254,252,123,32,32,32,32,32,32,32,32,108,160,160,126,225,32,124,251,160,123,32,32,108,254,254,160,127,127,251,160,32,32,32,251,97,126,32,32,32,254,252,123,32,32,32,32,32,32,32,32,108,160,160,126,225,32,124,251,160,123,32,32,108,254,254,160,127,127,251,160,32,32,32,251,97,126,32,32,32,254,252,123,32,32,32,32,32,32,32,32,108,160,160,126,225,32,124,251,160,123,32,32,108,254,254,160,127,127,251,160,32,32,32,251,97,126,32,32,32,254,252,123,32,32,32,32,32,32,32,32,108,160,160,126,225,32,124,251,160,123,32,32,108,254,254,160,127,127,251,160,32,32,32,251,97,126,32,32,32,254,252,123,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,108,126,32,124,123,252,123,32,32,32,32,32,32,
254,160,160,126,32,160,123,108,123,32,226,252,124,251,160,252,127,108,127,160,160,97,124,123,32,124,252,123,32,254,160,124,251,252,123,32,32,32,32,32,254,160,160,126,32,160,123,108,123,32,226,252,124,251,160,252,127,108,127,160,254,160,160,126,32,160,123,108,123,32,226,252,124,251,160,252,127,108,127,160,160,97,124,123,32,124,252,123,32,254,160,124,251,252,123,32,32,32,32,32,254,160,160,126,32,160,123,108,123,32,226,252,124,251,160,252,127,108,127,160,160,97,124,123,32,124,252,123,32,254,160,124,251,252,123,32,32,32,32,32,254,160,160,126,32,160,123,108,123,32,226,252,124,251,160,252,127,108,127,160,160,97,124,123,32,124,252,123,32,254,160,124,251,252,123,32,32,32,32,32,254,160,160,126,32,160,123,108,123,32,226,252,124,251,160,252,127,108,127,160,160,97,124,123,32,124,252,123,32,254,160,124,251,252,123,32,32,32,32,32,254,160,160,126,32,160,123,108,123,32,226,252,124,251,160,252,127,108,127,160,160,97,124,123,32,124,252,123,32,254,160,124,251,252,123,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,254,160,252,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,254,226,251,252,32,32,32,32,32,32,127,98,255,32,32,226,97,32,32,32,32,32,
160,160,252,160,225,160,252,32,127,251,123,124,251,98,124,251,252,252,160,160,160,160,123,251,252,123,32,32,225,160,160,127,32,251,160,252,32,32,32,254,160,160,252,160,225,160,252,32,127,251,123,124,251,98,124,251,252,252,160,160,160,160,252,160,225,160,252,32,127,251,123,124,251,98,124,251,252,252,160,160,160,160,123,251,252,123,32,32,225,160,160,127,32,251,160,252,32,32,32,254,160,160,252,160,225,160,252,32,127,251,123,124,251,98,124,251,252,252,160,160,160,160,123,251,252,123,32,32,225,160,160,127,32,251,160,252,32,32,32,254,160,160,252,160,225,160,252,32,127,251,123,124,251,98,124,251,252,252,160,160,160,160,123,251,252,123,32,32,225,160,160,127,32,251,160,252,32,32,32,254,160,160,252,160,225,160,252,32,127,251,123,124,251,98,124,251,252,252,160,160,160,160,123,251,252,123,32,32,225,160,160,127,32,251,160,252,32,32,32,254,160,160,252,160,225,160,252,32,127,251,123,124,251,98,124,251,252,252,160,160,160,160,123,251,252,123,32,32,225,160,160,127,32,251,160,252,32,32,32,254,123,108,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,225,160,127,124,97,32,32,32,225,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,160,32,225,160,32,32,32,32,32,98,98,123,32,98,32,32,32,32,32,32,32,32,
160,252,160,97,160,160,252,32,127,127,251,127,32,124,251,98,124,160,160,160,160,160,160,254,160,160,252,124,160,160,160,160,97,32,251,160,251,123,98,160,160,252,160,97,160,160,252,32,127,127,251,127,32,124,251,98,124,160,160,160,160,252,160,97,160,160,252,32,127,127,251,127,32,124,251,98,124,160,160,160,160,160,160,254,160,160,252,124,160,160,160,160,97,32,251,160,251,123,98,160,160,252,160,97,160,160,252,32,127,127,251,127,32,124,251,98,124,160,160,160,160,160,160,254,160,160,252,124,160,160,160,160,97,32,251,160,251,123,98,160,160,252,160,97,160,160,252,32,127,127,251,127,32,124,251,98,124,160,160,160,160,160,160,254,160,160,252,124,160,160,160,160,97,32,251,160,251,123,98,160,160,252,160,97,160,160,252,32,127,127,251,127,32,124,251,98,124,160,160,160,160,160,160,254,160,160,252,124,160,160,160,160,97,32,251,160,251,123,98,160,160,252,160,97,160,160,252,32,127,127,251,127,32,124,251,98,124,160,160,160,160,160,160,254,160,160,252,124,160,160,160,160,97,32,251,160,126,32,254,236,108,126,126,255,123,32,32,32,32,32,32,32,108,98,98,98,123,32,32,32,32,32,32,32,32,32,32,32,108,98,98,98,123,32,32,32,32,108,98,98,98,123,124,252,127,126,97,32,32,32,225,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,226,226,126,32,32,32,32,225,97,32,160,124,226,32,32,32,32,32,32,32,32,
160,160,252,160,160,160,160,252,32,127,127,251,251,251,98,124,252,98,160,160,160,160,160,252,160,160,160,160,254,160,160,160,160,225,252,160,160,160,236,160,160,160,252,160,160,160,160,252,32,127,127,251,251,251,98,124,252,98,160,160,160,160,252,160,160,160,160,252,32,127,127,251,251,251,98,124,252,98,160,160,160,160,160,252,160,160,160,160,254,160,160,160,160,225,252,160,160,160,236,160,160,160,252,160,160,160,160,252,32,127,127,251,251,251,98,124,252,98,160,160,160,160,160,252,160,160,160,160,254,160,160,160,160,225,252,160,160,160,236,160,160,160,252,160,160,160,160,252,32,127,127,251,251,251,98,124,252,98,160,160,160,160,160,252,160,160,160,160,254,160,160,160,160,225,252,160,160,160,236,160,160,160,252,160,160,160,160,252,32,127,127,251,251,251,98,124,252,98,160,160,160,160,160,252,160,160,160,160,254,160,160,160,160,225,252,160,160,160,236,160,160,160,252,160,160,160,160,252,32,127,127,251,251,251,98,124,252,98,160,160,160,160,160,252,160,160,160,160,254,160,160,160,160,225,252,236,32,254,236,254,126,32,255,32,32,32,32,32,32,32,32,108,160,251,254,127,226,123,32,32,32,32,32,32,32,32,32,108,160,251,254,127,226,123,32,32,108,160,251,254,127,226,123,252,127,126,97,32,32,32,225,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,254,226,251,252,32,32,124,252,98,236,124,226,32,32,32,32,32,32,32,32,
160,160,160,160,160,160,160,160,160,123,127,252,252,252,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,97,160,252,160,160,160,160,160,160,160,160,160,160,160,160,160,123,127,252,252,252,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,123,127,252,252,252,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,97,160,252,160,160,160,160,160,160,160,160,160,160,160,160,160,123,127,252,252,252,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,97,160,252,160,160,160,160,160,160,160,160,160,160,160,160,160,123,127,252,252,252,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,97,160,252,160,160,160,160,160,160,160,160,160,160,160,160,160,123,127,252,252,252,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,97,160,252,160,160,160,160,160,160,160,160,160,160,160,160,160,123,127,252,252,252,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,97,236,32,254,160,160,126,32,124,32,32,32,32,32,32,32,32,32,124,97,126,126,126,108,126,32,32,32,32,32,32,32,32,32,124,97,126,126,126,108,126,32,32,124,97,126,126,126,108,126,252,127,126,97,32,32,32,225,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,160,32,225,160,32,98,98,98,32,108,160,126,32,32,32,32,32,32,32,32

    
    
    };

void init_screen(void) {
    // Clear screen buffer first
    memset(screen_buffer, PETSCII_SPACE, SCREEN_COLS * SCREEN_ROWS);
    
    // Clear actual screen 
    memset((void*)SCREEN_RAM, PETSCII_SPACE, SCREEN_COLS * SCREEN_ROWS);
    
    // Initialize map pointer
    POKE(map_p, (unsigned int)map & 0xFF);
    POKE(map_p + 1, (unsigned int)map >> 8);

    // Initialize screen tables
    txt_DefineScreen();
}

void draw_screen(void) {
    unsigned char y;
    const unsigned char* map_ptr;
    unsigned char* screen_ptr = screen_buffer;
    
    // Clear buffer first
    memset(screen_buffer, PETSCII_SPACE, SCREEN_COLS * SCREEN_ROWS);
    
    // Copy screen data row by row
    // The map is 400 chars wide, so each row starts 400 bytes after the previous
    for(y = 0; y < SCREEN_ROWS; y++) {
        // Calculate start of this row in map
        // Each row is 400 bytes wide, and we offset by sx for scrolling
        map_ptr = map + (y * 400) + sx;
        
        // Copy just the visible 40 bytes from this row
        memcpy(screen_ptr, map_ptr, SCREEN_COLS);
        
        // Move to next row in screen buffer
        screen_ptr += SCREEN_COLS;
    }
}

void draw_ship(void) {
    unsigned char* ship_pos = screen_buffer + (y * SCREEN_COLS + x);
    
    // Disable collision detection to debug the issue
    // where the game immediately ends when the ship is on the map
    if(collision_off) {
    } else {
    // Check collision - check all three rows where ship will be
    if(ship_pos[0] != PETSCII_SPACE || ship_pos[1] != PETSCII_SPACE || 
       ship_pos[2] != PETSCII_SPACE || ship_pos[3] != PETSCII_SPACE ||
       ship_pos[4] != PETSCII_SPACE ||
       ship_pos[SCREEN_COLS] != PETSCII_SPACE || ship_pos[SCREEN_COLS+1] != PETSCII_SPACE ||
       ship_pos[SCREEN_COLS+2] != PETSCII_SPACE || ship_pos[SCREEN_COLS+3] != PETSCII_SPACE ||
       ship_pos[SCREEN_COLS+4] != PETSCII_SPACE ||
       ship_pos[SCREEN_COLS*2] != PETSCII_SPACE || ship_pos[SCREEN_COLS*2+1] != PETSCII_SPACE ||
       ship_pos[SCREEN_COLS*2+2] != PETSCII_SPACE || ship_pos[SCREEN_COLS*2+3] != PETSCII_SPACE ||
       ship_pos[SCREEN_COLS*2+4] != PETSCII_SPACE) {
        alive = 0;
        won_game = 0;
        return;
    }}
    
    // Draw ship
    memcpy(ship_pos, ship, 5);
    memcpy(ship_pos + SCREEN_COLS, ship + 5, 5);
    memcpy(ship_pos + (SCREEN_COLS * 2), ship + 10, 5);
}

void handle_input(void) {
    if(kbhit()) {
        char key = cgetc();
        old_x = x;
        old_y = y;
        
        switch(key) {
            case 'w':
            case 'W':
                if(y >= 2) {
                    y--;
                    if(y < 5) sy--;
                }
                break;
            case 'd':
            case 'D':
                if(x < SCREEN_COLS - 3) x++;
                break;
            case 's':
            case 'S':
                if(y < SCREEN_ROWS - 2) {
                    y++;
                    if(y >= 16) sy++;
                }
                break;
            case 'a':
            case 'A':
                if(x > 0) x--;
                break;
        }
    }
}

int main(void) {
    // Main game loop from assembly
    while(1) {
        // Initialize game state
        init_game();
        start_screen();
        
        // Game loop
        while(alive && !won_game) {
            // Wait for vertical blank
            wait_vsync();
            
            // Handle input
            handle_input();
            
            // Update game state
            ticks++;
            if((ticks % 5) == 0) {
                // Scroll map - increment by 1 but wrap at 400 since that's the map width
                sx++;
                if(sx >= 400) sx = 0;
                
                // Draw new frame
                draw_screen();
                draw_ship();
                
                // Update screen
                memcpy((void*)SCREEN_RAM, screen_buffer, SCREEN_COLS * SCREEN_ROWS);
            }
        }
        
        // Show end screen
        end_screen();
    }
    
    return 0;
}

void init_game(void) {
    // Initialize screen
    init_screen();
    
    // Initialize game state
    sx = 0;
    sy = 0;
    x = 4;
    y = 10;
    alive = 1;
    won_game = 0;
    ticks = 0;
    
    // Clear keyboard state
    memset(Key_keys, 0, sizeof(Key_keys));
    memset(Key_keysLast, 0, sizeof(Key_keysLast));
    memset(Key_keysPressed, 0, sizeof(Key_keysPressed));
    Key_keyState = 0;
    Key_temp = 0;
} 