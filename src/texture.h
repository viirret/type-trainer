#ifndef TEXTURE_HH
#define TEXTURE_HH

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
} Texture;

void Texture_init(Texture* texture, SDL_Renderer* renderer, TTF_Font* font, const char* word, SDL_Color color);
void Texture_destroy(Texture* texture);
void Texture_render(Texture* texture, SDL_Renderer* renderer, int x, int y, SDL_Color color);
void Texture_update(Texture* texture, SDL_Renderer* renderer, TTF_Font* font, char* newChars, SDL_Color color);

#endif
