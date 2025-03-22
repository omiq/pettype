#ifndef GAME_H
#define GAME_H

#include "../engine/graphics.h"
#include "player.h"

// Game states
enum GameState {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_GAME_OVER
};

// Game management
void init_game(void);
void update_game(void);
void render_game(void);
void handle_input(void);

#endif 