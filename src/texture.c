#include "texture.h"

void Texture_init(Texture* texture, SDL_Renderer* renderer, TTF_Font* font, const char* word, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, word, color);
    texture->texture = SDL_CreateTextureFromSurface(renderer, surface);
    texture->width = surface->w;
    texture->height = surface->h;
    SDL_FreeSurface(surface);
}

void Texture_destroy(Texture* tex) {
    SDL_DestroyTexture(tex->texture);
}

void Texture_render(Texture* tex, SDL_Renderer* renderer, int x, int y, SDL_Color color) {
    SDL_SetTextureColorMod(tex->texture, color.r, color.g, color.b);
    SDL_Rect renderQuad = {x, y, tex->width, tex->height};
    SDL_RenderCopy(renderer, tex->texture, NULL, &renderQuad);
}
