#include "game.h"
#include "texture.h"
#include "window.h"
#include "word.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <SDL2/SDL_ttf.h>

// Define colors
const SDL_Color COLOR_REGULAR = {255, 255, 255, 255};
const SDL_Color COLOR_ERROR = {255, 0, 0, 255};
const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color COLOR_GRAY = {64, 64, 64, 255};

void Game_init(Game* game) {
    game->font = TTF_OpenFont("/usr/share/fonts/gnu-free/FreeMono.otf", 28);
    game->checkIndex = 0;
    game->failures = 0;
    game->lastLetter = 1000;
    game->close = false;
    game->drawColor = COLOR_REGULAR;
    game->sentence = NULL;
    game->colors = NULL;
    clock_gettime(CLOCK_REALTIME, &game->startTime);

    if (!game->font) {
        SDL_Log("Failed to load the font! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }
    Window_init(&game->window);

    // Initialize accuracy.
    Texture_init(&game->accuracyTexture, game->window.renderer, game->font, Game_getAccuracy(game), COLOR_ERROR);

    // Initialize wpm.
    //Texture_init(&game->speedTexture, game->window.renderer, game->font, Game_getSpeed(game), COLOR_REGULAR);

    Word_init(&game->word);
}

void Game_destroy(Game* game) {
    Texture_destroy(&game->accuracyTexture);
    //Texture_destroy(&game->speedTexture);
    //Texture_destroy(&game->traineeText);
    Word_destroy(&game->word);
    if (game->sentence) {
        free(game->sentence);
    }
    if (game->colors) {
        free(game->colors);
    }
    Window_destroy(&game->window);
    TTF_CloseFont(game->font);
}

void Game_update(Game* game) {
    Game_eventHandler(game);
    Game_render(game);
}

void Game_render(Game* game) {
    Window_setColor(&game->window, COLOR_GRAY);
    Window_clear(&game->window);

    Texture_render(&game->accuracyTexture, game->window.renderer, 0, 0, COLOR_WHITE);
    //Texture_render(&game->speedTexture, game->window.renderer, 0, 0, COLOR_WHITE);

    Window_render(&game->window);
}

void Game_eventHandler(Game* game) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        Window_resize(&game->window, e);

        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
            game->close = true;
        }
    }
}

void Game_restart(Game* game) {
    clock_gettime(CLOCK_REALTIME, &game->endTime);
    Game_setup(game);
    Game_createSentence(game, 10);
    clock_gettime(CLOCK_REALTIME, &game->startTime);
}

char* Game_getAccuracy(Game* game) {
    static char accuracy[20];
    double acc = game->failures > 0 ? (1 - (double)game->failures / (double)game->lastLetter) * 100 : 100;
    snprintf(accuracy, sizeof(accuracy), "Accuracy: %.2f", acc);
    return accuracy;
}

char* Game_getSpeed(Game* game) {
    static char speed[20];
    double seconds = (game->endTime.tv_sec - game->startTime.tv_sec) +
                     (game->endTime.tv_nsec - game->startTime.tv_nsec) / 1e9;
    snprintf(speed, sizeof(speed), "Speed: %.2f wpm", 60.0 / seconds * 10);
    return speed;
}

void Game_setup(Game* game) {
    game->failures = 0;
    game->checkIndex = 0;
}

char* Game_createSentence(Game* game, int wordCount) {
    return Word_getSentence(&game->word, wordCount);
}
