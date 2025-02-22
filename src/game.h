#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "texture.h"
#include "window.h"
#include "word.h"
#include "game_metrics.h"

#include <time.h>
#include <stdint.h>

#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
    Texture accuracyTexture;
    Texture speedTexture;
} MetricsTextures;

typedef struct {
    uint32_t failures;
    uint32_t lastLetter;
} Accuracy;

typedef struct {
    MetricsTextures textures;
    GameMetrics metrics;
    Accuracy accuracy;
} Metrics;

typedef struct {
    // Game config.
    Config config;

    // Window and SDL context.
    Window window;

    // Dictionary file used for words.
    Word word;

    // Program font.
    TTF_Font* font;

    // Metrics data
    Metrics metrics;

    // The writable text.
    char* sentence;
    Texture* textures;
    SDL_Color* colors;

    // Index the game is currently writing next.
    uint32_t checkIndex;

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
