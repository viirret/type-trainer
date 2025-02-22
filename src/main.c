#include "game.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    if (!TTF_Init()) {
        SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Game game;
    Game_init(&game);
    Game_setup(&game);

    while (!game.close) {
        Game_update(&game);
        SDL_Delay(16); // 60 fps
    }

    Game_destroy(&game);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
