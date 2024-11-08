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
    int totalWordsPerGame;
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
extern const SDL_Color COLOR_GRAY;

// Allocate for game.
void Game_init(Game* game);

// Setup for new game.
void Game_setup(Game* game);

// Deallocate game.
void Game_destroy(Game* game);

// Update game loop.
void Game_update(Game* game);

// Recreate texture objects.
void Game_initTextures(Game* game);

void Game_destroyTextures(Game* game);
void Game_restart(Game* game);
char* Game_getAccuracy(Game* game);
char* Game_getSpeed(Game* game);

// Timing functions.
void _Game_start(Game* game);
void _Game_end(Game* game);

/*
 *  Update loop
 */

// Event handling
void _Game_eventHandler(Game* game);

// Rendering.
void Game_render(Game* game);
void Game_renderText(Game* game);
void Game_renderMetrics(Game* game);

#endif // GAME_H
