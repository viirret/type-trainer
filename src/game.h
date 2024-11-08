#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "texture.h"
#include "window.h"
#include "word.h"

#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

typedef struct {
    Config config;
    Window window;
    TTF_Font* font;
    Word word;
    Texture accuracyTexture;
    Texture speedTexture;
    char* sentence;
    Texture* textures;
    SDL_Color* colors;

    int checkIndex;
    int failures;
    int lastLetter;
    bool close;
    struct timespec startTime;
    struct timespec endTime;
    bool shiftPressed;
} Game;

// Color constants
extern const SDL_Color COLOR_BLACK;
extern const SDL_Color COLOR_WHITE;
extern const SDL_Color COLOR_ERROR;

// Allocate for game.
void Game_init(Game* game);

// Setup for new game.
void Game_setup(Game* game);

// Deallocate game.
void Game_destroy(Game* game);

// Update game loop.
void Game_update(Game* game);

#endif // GAME_H
