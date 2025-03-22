#include <peekpoke.h>

#define SCREEN_RAM 0x8000

// Player ship sprite (2x2 PETSCII characters)
const unsigned char player_sprite[] = {
    0xE4, 0xE9, // Left and right parts of ship
    0xEC, 0xEB  // Bottom parts of ship
};

// Bullet properties
#define MAX_BULLETS 8
struct Bullet {
    unsigned char x;
    unsigned char y;
    char active;
} bullets[MAX_BULLETS];

// Charge shot properties
unsigned char charge_level;
#define MAX_CHARGE 3

void init_player(void) {
    unsigned char i;
    for(i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
    charge_level = 0;
}

void fire_bullet(unsigned char x, unsigned char y) {
    unsigned char i;
    // Find first inactive bullet
    for(i = 0; i < MAX_BULLETS; i++) {
        if(!bullets[i].active) {
            bullets[i].x = x + 2; // Start at ship's nose
            bullets[i].y = y;
            bullets[i].active = 1;
            break;
        }
    }
}

void update_bullets(void) {
    unsigned char i;
    for(i = 0; i < MAX_BULLETS; i++) {
        if(bullets[i].active) {
            bullets[i].x += 2;
            if(bullets[i].x >= 39) { // Screen width limit
                bullets[i].active = 0;
            }
        }
    }
}

void charge_weapon(void) {
    if(charge_level < MAX_CHARGE) {
        charge_level++;
    }
}

void release_charge_shot(unsigned char x, unsigned char y) {
    unsigned char i;
    if(charge_level > 0) {
        // Fire a larger bullet based on charge level
        for(i = 0; i < charge_level; i++) {
            if(y > 0) fire_bullet(x, y - 1);
            fire_bullet(x, y);
            if(y < 24) fire_bullet(x, y + 1);
        }
        charge_level = 0;
    }
}

void draw_player(unsigned char x, unsigned char y) {
    // Draw player ship using PETSCII characters
    POKE(SCREEN_RAM + y * 40 + x, player_sprite[0]);
    POKE(SCREEN_RAM + y * 40 + x + 1, player_sprite[1]);
    POKE(SCREEN_RAM + (y + 1) * 40 + x, player_sprite[2]);
    POKE(SCREEN_RAM + (y + 1) * 40 + x + 1, player_sprite[3]);
}

void draw_bullets(void) {
    unsigned char i;
    for(i = 0; i < MAX_BULLETS; i++) {
        if(bullets[i].active) {
            POKE(SCREEN_RAM + bullets[i].y * 40 + bullets[i].x, 0xE2);
        }
    }
} 