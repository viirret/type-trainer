#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    Game game;
    Game_init(&game);

    // Set up the game
    Game_setup(&game);

    //char* sentence = Game_createSentence(&game, 100);
    //printf("%s", sentence);

    // Main game loop
    while (!game.close) {
        Game_update(&game);
        SDL_Delay(16);  // Delay to cap the frame rate (about 60 frames per second)
    }

    // Clean up and close
    Game_destroy(&game);

    // Quit SDL and SDL_ttf
    TTF_Quit();
    SDL_Quit();

    return 0;
}
