#include "game.h"
#include "config.h"
#include "texture.h"
#include "word.h"

#include <time.h>

const SDL_Color COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color COLOR_ERROR = {255, 0, 0, 255};
const SDL_Color COLOR_GRAY = {64, 64, 64, 255};

void Game_init(Game* game) {
    Config_load(&game->config);
    Window_init(&game->window);
    game->font = TTF_OpenFont(game->config.font.value.str_value, game->config.path_size.value.int_value);
    if (!game->font) {
        SDL_Log("Failed to load the font! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }
    Word_init(&game->word);
    game->totalWordsPerGame = game->config.total_words.value.int_value;
    Texture_init(&game->accuracyTexture, game->window.renderer, game->font, "Accuracy", COLOR_WHITE);
    Texture_init(&game->speedTexture, game->window.renderer, game->font, "Speed", COLOR_WHITE);
}

void Game_initTextures(Game* game) {
    game->sentence = strdup(Word_getSentence(&game->word, game->totalWordsPerGame));
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
}

void Game_destroy(Game* game) {
    Config_destroy(&game->config);
    Word_destroy(&game->word);

    Game_destroyTextures(game);

    Texture_destroy(&game->accuracyTexture);
    Texture_destroy(&game->speedTexture);

    Window_destroy(&game->window);
    TTF_CloseFont(game->font);
}

void Game_destroyTextures(Game* game) {
    for (size_t i = 0; i < strlen(game->sentence); i++) {
        Texture_destroy(&game->textures[i]);
    }
    free(game->colors);
    free(game->sentence);
}

void Game_update(Game* game) {
    _Game_eventHandler(game);
    Game_render(game);
}

void _Game_eventHandler(Game* game) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        Window_resize(&game->window, e);

        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && ((e.key.keysym.sym == SDLK_ESCAPE) || (e.key.keysym.sym == SDLK_ESCAPE)))) {
            game->close = true;
        }
        else if (e.type == SDL_KEYDOWN) {
            fflush(stdout);

            int key = e.key.keysym.sym;
            if (key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
                game->shiftPressed = true;
                continue;
            }
            if (key == SDLK_LGUI) {
                printf("Left Super key (LGUI) pressed\n");
                continue;
            }
            if (key== SDLK_RGUI) {
                printf("Right Super key (RGUI) pressed\n");
                continue;
            }

            char expectedChar = game->sentence[game->checkIndex];
            char* underscore = {"_"};

            bool correct = (game->shiftPressed && toupper(key) == expectedChar) ||
                           (!game->shiftPressed && key == expectedChar);

            if (correct) {
                printf("Correct! Input: %c, expected char: %c\n", key, expectedChar);

                // Convert spaces to underscore.
                if (expectedChar == 32) {
                    Texture_update(&game->textures[game->checkIndex], game->window.renderer, game->font, underscore, COLOR_BLACK);
                }
                game->colors[game->checkIndex] = COLOR_BLACK;
                game->checkIndex++;

                if (game->shiftPressed) {
                    game->shiftPressed = false;
                }

                if (game->checkIndex >= game->lastLetter) {
                    Game_restart(game);
                }
            }
            else {
                printf("Incorrect! Input char: %c, expected char: %c \n", key, expectedChar);
                if (expectedChar == 32) {
                    Texture_update(&game->textures[game->checkIndex], game->window.renderer, game->font, underscore, COLOR_ERROR);
                }
                game->colors[game->checkIndex] = COLOR_ERROR;
                game->checkIndex++;
                game->failures++;

                if (game->shiftPressed) {
                    game->shiftPressed = false;
                }

                if (game->checkIndex >= game->lastLetter) {
                    Game_restart(game);
                }
            }
        }
    }
}

void Game_restart(Game* game) {
    _Game_end(game);
    Game_setup(game);
}

char* Game_getAccuracy(Game* game) {
    printf("Failures in accuracy: %d\n", game->failures);
    static char accuracy[22];

    // Check if there is anything to count
    if (game->lastLetter == 0) {
        snprintf(accuracy, sizeof(accuracy), "No record of accuracy");
    } else {
        // Calculate accuracy percentage
        double acc = game->failures > 0 ? (1 - (double)game->failures / (double)game->lastLetter) * 100 : 100;
        snprintf(accuracy, sizeof(accuracy), "Last accuracy: %.2f", acc);
    }
    return accuracy;
}

char* Game_getSpeed(Game* game) {
    static char speed[20];

    if (game->endTime.tv_sec < 0) {
        snprintf(speed, sizeof(speed), "No last speed");
    }

    // Calculate the difference in seconds and nanoseconds
    double seconds = (double)(game->endTime.tv_sec - game->startTime.tv_sec) +
                     (double)(game->endTime.tv_nsec - game->startTime.tv_nsec) / 1e9;
    if (seconds > 0) {
        double wpm = game->totalWordsPerGame / (seconds / 60.0);
        snprintf(speed, sizeof(speed), "Last speed: %.2f", wpm);
    } else {
        snprintf(speed, sizeof(speed), "No record of speed");
    }
    return speed;
}

void Game_setup(Game* game) {
    Game_initTextures(game);
    Texture_update(&game->accuracyTexture, game->window.renderer, game->font, Game_getAccuracy(game), COLOR_WHITE);
    Texture_update(&game->speedTexture, game->window.renderer, game->font, Game_getSpeed(game), COLOR_WHITE);
    game->checkIndex = 0;
    game->failures = 0;
    game->lastLetter = strlen(game->sentence);
    game->close = false;
    game->shiftPressed = false;
    _Game_start(game);
}

void Game_render(Game* game) {
    Window_setColor(&game->window, game->config.background_color.value.color_value);
    Window_clear(&game->window);

    if (!game->window.tooSmall) {
        Game_renderText(game);
        Game_renderMetrics(game);
    }
    Window_render(&game->window);
}

void Game_renderText(Game* game) {
    size_t sentenceLen = strlen(game->sentence);
    int firstLineIndex = 0, secondLineIndex = 0, thirdLineIndex = 0;
    for (size_t i = 0; i < sentenceLen; i++) {
        int w = game->textures[i].width;
        int xpadding = 100;
        int ypadding = 400;
        size_t estimate = game->window.width - xpadding * 2;

        if (i < estimate) {
            Texture_render(&game->textures[i], game->window.renderer, xpadding + firstLineIndex * w, ypadding, game->colors[i]);
            firstLineIndex++;
        }
        else if (i >= estimate && i < 2 * estimate) {
            Texture_render(&game->textures[i], game->window.renderer, xpadding + secondLineIndex * w, ypadding + 100, game->colors[i]);
            secondLineIndex++;
        }
        else if (i >= estimate * 2 && i < 3 * estimate) {
            Texture_render(&game->textures[i], game->window.renderer, xpadding + thirdLineIndex * w, ypadding + 200, game->colors[i]);
            thirdLineIndex++;
        }
    }
}

void Game_renderMetrics(Game* game) {
    int w = game->window.width;
    int h = game->window.height;
    Texture_render(&game->accuracyTexture, game->window.renderer, w / 10, h / 10, COLOR_WHITE);
    Texture_render(&game->speedTexture, game->window.renderer, w - w / 4, h / 10, COLOR_WHITE);
}

void _Game_start(Game* game) {
    clock_gettime(CLOCK_REALTIME, &game->startTime);
}

void _Game_end(Game* game) {
    clock_gettime(CLOCK_REALTIME, &game->endTime);
}
