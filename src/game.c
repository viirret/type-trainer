#include "game.h"
#include "config.h"
#include "config_file.h"

void initTextures(Game* game) {
    if (!game->config.total_words.is_set) {
        perror("Cannot continue");
    }
    game->sentence = strdup(Word_getSentence(&game->word, game->config.total_words.value.int_value));
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
        Texture_init(&game->textures[i], game->window.renderer, game->font, letter, game->config.color_text_default.value.color_value);
        game->colors[i] = game->config.color_text_default.value.color_value;
        i++;
    }
}

void startGame(Game* game) {
    clock_gettime(CLOCK_REALTIME, &game->startTime);
}

void endGame(Game* game) {
    clock_gettime(CLOCK_REALTIME, &game->endTime);
}

void Game_init(Game* game) {
    Config_init(&game->config);
    Window_init(&game->window);
    Word_init(&game->word, game->config.dictionary.value.str_value);
    game->font = TTF_OpenFont(game->config.font.value.str_value, game->config.font_size.value.int_value);
    if (!game->font) {
        SDL_Log("Failed to load the font! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }
    createConfigFiles();
    GameMetrics_init(&game->metrics.metrics);
    GameMetrics_loadAccuracy(&game->metrics.metrics, ConfigFileResolve(CONFIG_DATA_FILE_ACCURACY));
    GameMetrics_loadSpeed(&game->metrics.metrics, ConfigFileResolve(CONFIG_DATA_FILE_SPEED));

    char accuracy[50];
    char speed[50];
    snprintf(accuracy, sizeof(accuracy), "Last accuracy: %.2f", GameMetrics_getAverageAccuracy(&game->metrics.metrics));
    snprintf(speed, sizeof(speed), "Last speed: %.2f", GameMetrics_getAverageSpeed(&game->metrics.metrics));
    Texture_init(&game->metrics.textures.accuracyTexture, game->window.renderer, game->font, accuracy, game->config.color_text_default.value.color_value);
    Texture_init(&game->metrics.textures.speedTexture, game->window.renderer, game->font, speed, game->config.color_text_default.value.color_value);
}

void destroyTextures(Game* game) {
    for (size_t i = 0; i < strlen(game->sentence); i++) {
        Texture_destroy(&game->textures[i]);
    }
    free(game->colors);
    free(game->sentence);
}

void Game_destroy(Game* game) {
    Word_destroy(&game->word);

    destroyTextures(game);

    Texture_destroy(&game->metrics.textures.accuracyTexture);
    Texture_destroy(&game->metrics.textures.speedTexture);

    Window_destroy(&game->window);
    TTF_CloseFont(game->font);
}

void renderText(Game* game) {
    size_t sentenceLen = strlen(game->sentence);
    int xpadding = 100;
    int ypadding = xpadding * 4;
    int maxLineWidth = game->window.width - xpadding * 2;

    int currentX = xpadding;
    int currentY = ypadding;
    int lineIndex = 0;

    for (size_t i = 0; i < sentenceLen; i++) {
        int w = game->textures[i].width;

        // If adding this character would exceed the max width, move to the next line
        if (currentX + w > xpadding + maxLineWidth) {
            currentX = xpadding;
            lineIndex++;
            currentY = ypadding + lineIndex * xpadding;
        }

        // Render the texture at the current position
        Texture_render(&game->textures[i], game->window.renderer, currentX, currentY, game->colors[i]);

        // Advance to the next character's position
        currentX += w;
    }
}

void renderMetrics(Game* game) {
    int w = game->window.width;
    int h = game->window.height;
    int offset_x = w / 10;
    int offset_y = h / 10;
    SDL_Color text_color = game->config.color_text_default.value.color_value;

    Texture_render(&game->metrics.textures.speedTexture, game->window.renderer, offset_x, offset_y, text_color);
    Texture_render(&game->metrics.textures.accuracyTexture, game->window.renderer, w - offset_x - game->metrics.textures.speedTexture.width, offset_y, text_color);
}

void render(Game* game) {
    Window_setColor(&game->window, game->config.color_background.value.color_value);
    Window_clear(&game->window);

    if (!game->window.tooSmall) {
        renderText(game);
        renderMetrics(game);
    }
    Window_render(&game->window);
}

double gameDuration(Game* game) {
    return (double)(game->endTime.tv_sec - game->startTime.tv_sec) +
        (double)(game->endTime.tv_nsec - game->startTime.tv_nsec) / 1e9;
}

double gameWpm(Game* game, double duration) {
    return game->config.total_words.value.int_value / (duration / 60.0);
}

double gameAccuracy(Game* game) {
    return game->metrics.accuracy.failures > 0 ? (1 - (double)game->metrics.accuracy.failures / (double)game->metrics.accuracy.lastLetter) * 100 : 100;
}

void restart(Game* game) {
    endGame(game);
    fflush(stdout);
    printf("Game over!\n");
    double game_duration = gameDuration(game);
    double game_wpm = gameWpm(game, game_duration);
    double game_accuracy = gameAccuracy(game);

    ConfigFileWriteInt(CONFIG_DATA_FILE_ACCURACY, game_accuracy);
    ConfigFileWriteInt(CONFIG_DATA_FILE_SPEED, game_wpm);

    Game_setup(game);
}

void updateWrittenKey(Game* game, char expected_char, bool isCorrect) {
    char* underscore = {"_"};
    SDL_Color color = isCorrect ? game->config.color_text_typed.value.color_value :
                                  game->config.color_text_error.value.color_value;
    // Convert spaces to underscore.
    if (expected_char == 32) {
        Texture_update(&game->textures[game->checkIndex], game->window.renderer, game->font, underscore, color);
    }
    if (isCorrect) {
        game->colors[game->checkIndex] = color;
        game->checkIndex++;
    }
    else {
        game->colors[game->checkIndex] = color;
        if (game->config.advance_on_failure.value.boolean_value) {
            game->checkIndex++;
        }
        game->metrics.accuracy.failures++;
    }
    if (game->shiftPressed) {
        game->shiftPressed = false;
    }
    if (game->checkIndex >= game->metrics.accuracy.lastLetter) {
        restart(game);
    }
}

void updateMetricsTextures(Game* game) {
    char accuracy[50];
    char speed[50];

    GameMetrics_loadAccuracy(&game->metrics.metrics, ConfigFileResolve(CONFIG_DATA_FILE_ACCURACY));
    GameMetrics_loadSpeed(&game->metrics.metrics, ConfigFileResolve(CONFIG_DATA_FILE_SPEED));

    snprintf(accuracy, sizeof(accuracy), "Last accuracy: %.2f", GameMetrics_getAverageAccuracy(&game->metrics.metrics));
    snprintf(speed, sizeof(speed), "Last speed: %.2f", GameMetrics_getAverageSpeed(&game->metrics.metrics));
    Texture_update(&game->metrics.textures.accuracyTexture, game->window.renderer, game->font, accuracy, game->config.color_text_default.value.color_value);
    Texture_update(&game->metrics.textures.speedTexture, game->window.renderer, game->font, speed, game->config.color_text_default.value.color_value);
}

void eventHandler(Game* game) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        Window_resize(&game->window, e);

        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_ESCAPE))) {
            game->close = true;
            Game_destroy(game);
            exit(0);
            return;
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
            if (key == SDLK_RGUI) {
                printf("Right Super key (RGUI) pressed\n");
                continue;
            }

            char expected_char = game->sentence[game->checkIndex];
            bool correct = (game->shiftPressed && toupper(key) == expected_char) ||
                           (!game->shiftPressed && key == expected_char);

            if (correct) {
                printf("Correct! Input: %c, expected char: %c\n", key, expected_char);
                updateWrittenKey(game, expected_char, correct);
            }
            else {
                printf("Incorrect! Input char: %c, expected char: %c \n", key, expected_char);
                updateWrittenKey(game, expected_char, correct);
            }
        }
    }
}

void Game_update(Game* game) {
    eventHandler(game);
    render(game);
}

void Game_setup(Game* game) {
    initTextures(game);

    updateMetricsTextures(game);
    game->checkIndex = 0;
    game->metrics.accuracy.failures = 0;
    game->metrics.accuracy.lastLetter = strlen(game->sentence);
    game->close = false;
    game->shiftPressed = false;
    startGame(game);
}

