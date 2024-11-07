#ifndef GAME_H
#define GAME_H

#include "texture.h"
#include "window.h"
#include "word.h"

#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

#define TOTAL_WORDS_PER_GAME 3

typedef struct {
    Window window;
    TTF_Font* font;
    Word word;
    char* sentence;
    Texture* textures;
    SDL_Color* colors;
    Texture accuracyTexture;
    Texture speedTexture;

    int checkIndex;
    int failures;
    int lastLetter;
    bool close;
    struct timespec startTime;
    struct timespec endTime;
} Game;

// Color constants
extern const SDL_Color COLOR_BLACK;
extern const SDL_Color COLOR_WHITE;
extern const SDL_Color COLOR_ERROR;
extern const SDL_Color COLOR_GRAY;

void Game_init(Game* game);
void Game_setup(Game* game);
void Game_destroy(Game* game);
void Game_update(Game* game);

void Game_initTextures(Game* game);
void Game_eventHandler(Game* game);
void Game_restart(Game* game);
char* Game_getAccuracy(Game* game);
char* Game_getSpeed(Game* game);
char* Game_createSentence(Game* game, int wordCount);

// Rendering
void Game_render(Game* game);
void Game_renderText(Game* game);
void Game_renderMetrics(Game* game);

#endif // GAME_H
