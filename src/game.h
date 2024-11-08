#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "texture.h"
#include "window.h"
#include "word.h"

#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

typedef struct {
    Texture accuracyTexture;
    Texture speedTexture;
} MetricsTextures;

typedef struct {
    // Game config.
    Config config;

    // Window and SDL context.
    Window window;

    // Dictionary file used for words.
    Word word;

    // Program font.
    TTF_Font* font;

    // Textures for displaying metrics data.
    MetricsTextures metrics_textures;

    // The writable text.
    char* sentence;
    Texture* textures;
    SDL_Color* colors;

    // Game logic variables.
    int checkIndex;
    int failures;
    int lastLetter;

    // Time measurement.
    struct timespec startTime;
    struct timespec endTime;

    // Is the shift-key pressed for capital letters.
    bool shiftPressed;

    // The application should close.
    bool close;
} Game;

// Allocate for game.
void Game_init(Game* game);

// Setup for new game.
void Game_setup(Game* game);

// Deallocate game.
void Game_destroy(Game* game);

// Update game loop.
void Game_update(Game* game);

#endif // GAME_H
