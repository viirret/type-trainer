#include "game.h"
#include "texture.h"

#include <stdio.h>
#include <time.h>

const SDL_Color COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color COLOR_ERROR = {255, 0, 0, 255};
const SDL_Color COLOR_GRAY = {64, 64, 64, 255};

void Game_init(Game* game) {
    Window_init(&game->window);
    game->font = TTF_OpenFont("/usr/share/fonts/gnu-free/FreeMono.otf", 28);
    if (!game->font) {
        SDL_Log("Failed to load the font! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }
    Word_init(&game->word);
}

void Game_initTextures(Game* game) {
    game->sentence = strdup(Game_createSentence(game, TOTAL_WORDS_PER_GAME));
    game->textures = malloc(sizeof(Texture) * strlen(game->sentence));

    game->colors = malloc(sizeof(int) * strlen(game->sentence));
    if (!game->sentence || !game->textures || !game->colors) {
        fprintf(stderr, "Memory allocation for texture data.\n");
        return;
    }

    // Initialize a texture for each character in the sentence.
    int i = 0;
    for (const char *p = game->sentence; *p != '\0'; p++) {
        char letter[2] = {*p, '\0'};
        Texture_init(&game->textures[i], game->window.renderer, game->font, letter, COLOR_WHITE);
        game->colors[i] = COLOR_WHITE;
        i++;
    }

    Texture_init(&game->accuracyTexture, game->window.renderer, game->font, Game_getAccuracy(game), COLOR_WHITE);
    Texture_init(&game->speedTexture, game->window.renderer, game->font, Game_getSpeed(game), COLOR_WHITE);
}

void Game_destroy(Game* game) {
    Word_destroy(&game->word);

    for (size_t i = 0; i < strlen(game->sentence); i++) {
        Texture_destroy(&game->textures[i]);
    }
    Texture_destroy(&game->accuracyTexture);
    Texture_destroy(&game->speedTexture);

    free(game->colors);
    free(game->sentence);

    Window_destroy(&game->window);
    TTF_CloseFont(game->font);
}

void Game_update(Game* game) {
    Game_eventHandler(game);
    Game_render(game);
}

void Game_eventHandler(Game* game) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        Window_resize(&game->window, e);

        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && ((e.key.keysym.sym == SDLK_ESCAPE) || (e.key.keysym.sym == SDLK_ESCAPE)))) {
            game->close = true;
        }
        else if (e.type == SDL_KEYDOWN) {
            fflush(stdout);

            // Correct letter.
            if (e.key.keysym.sym == game->sentence[game->checkIndex]) {
                printf("Correct key\n");
                if (game->sentence[game->checkIndex] == ' ') {
                    Texture_update(&game->textures[game->checkIndex], game->window.renderer, game->font, '_', COLOR_BLACK);
                }
                game->colors[game->checkIndex] = COLOR_BLACK;
                game->checkIndex++;

                if (game->checkIndex >= game->lastLetter) {
                    Game_restart(game);
                }
            }
            // Incorrect letter.
            else {
                printf("Incorrect letter\n");
                if (game->sentence[game->checkIndex] == ' ') {
                    Texture_update(&game->textures[game->checkIndex], game->window.renderer, game->font, '_', COLOR_ERROR);
                }
                game->colors[game->checkIndex] = COLOR_ERROR;
                game->checkIndex++;

                if (game->checkIndex >= game->lastLetter) {
                    Game_restart(game);
                }
            }
        }
    }
}

void Game_restart(Game* game) {
    clock_gettime(CLOCK_REALTIME, &game->endTime);
    Game_setup(game);
    clock_gettime(CLOCK_REALTIME, &game->startTime);
}

char* Game_getAccuracy(Game* game) {
    static char accuracy[20];
    double acc = game->failures > 0 ? (1 - (double)game->failures / (double)game->lastLetter) * 100 : 100;
    snprintf(accuracy, sizeof(accuracy), "Last accuracy: %.2f", acc);
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
    Game_initTextures(game);
    game->failures = 0;
    game->checkIndex = 0;
    game->lastLetter = strlen(game->sentence);
    game->close = false;
}

char* Game_createSentence(Game* game, int wordCount) {
    return Word_getSentence(&game->word, wordCount);
}

void Game_render(Game* game) {
    Window_setColor(&game->window, COLOR_GRAY);
    Window_clear(&game->window);

    Game_renderText(game);
    Game_renderMetrics(game);
    Window_render(&game->window);
}

void Game_renderText(Game* game) {
    for (size_t i = 0; i < strlen(game->sentence); i++) {
        int w = game->textures[i].width;
        Texture_render(&game->textures[i], game->window.renderer, game->window.width / 20 + i * w, game->window.height/ 2.25, game->colors[i]);
    }
}

void Game_renderMetrics(Game* game) {
    int w = game->window.width;
    int h = game->window.height;
    Texture_render(&game->accuracyTexture, game->window.renderer, w / 10, h / 10, COLOR_WHITE);
    Texture_render(&game->speedTexture, game->window.renderer, w - w / 10, h / 10, COLOR_WHITE);
}
