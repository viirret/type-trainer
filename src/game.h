#ifndef GAME_H
#define GAME_H

#include "texture.h"
#include "window.h"
#include "word.h"

#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

typedef struct {
    Window window;
    TTF_Font* font;
    Texture accuracyTexture;
    Texture speedTexture;
    Texture traineeText;
    Word word;
    SDL_Color drawColor;
    int checkIndex;
    int failures;
    int lastLetter;
    bool close;
    char* sentence;
    SDL_Color* colors;
    struct timespec startTime;
    struct timespec endTime;
} Game;

// Color constants
extern const SDL_Color COLOR_REGULAR;
extern const SDL_Color COLOR_ERROR;
extern const SDL_Color COLOR_WHITE;

// Game functions
void Game_init(Game* game);
void Game_destroy(Game* game);
void Game_update(Game* game);
void Game_render(Game* game);
void Game_eventHandler(Game* game);
void Game_restart(Game* game);
char* Game_getAccuracy(Game* game);
char* Game_getSpeed(Game* game);
void Game_setup(Game* game);
char* Game_createSentence(Game* game, int wordCount);

#endif // GAME_H
