#include "texture.h"

void Texture_init(Texture* texture, SDL_Renderer* renderer, TTF_Font* font, const char* word, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, word, strlen(word), color);
    texture->texture = SDL_CreateTextureFromSurface(renderer, surface);
    float w, h;
    SDL_GetTextureSize(texture->texture, &w, &h);
    texture->width = (int)w;
    texture->height = (int)h;
    SDL_DestroySurface(surface);
}

void Texture_destroy(Texture* tex) {
    SDL_DestroyTexture(tex->texture);
}

void Texture_render(Texture* tex, SDL_Renderer* renderer, int x, int y, SDL_Color color) {
    SDL_SetTextureColorMod(tex->texture, color.r, color.g, color.b);
    SDL_FRect renderQuad = {x, y, tex->width, tex->height};
    SDL_RenderTexture(renderer, tex->texture, NULL, &renderQuad);
}

void createTexture(Texture* texture, SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color color) {
    // Destroy the previous texture
    if (texture->texture) {
        SDL_DestroyTexture(texture->texture);
    }

    // Create a new surface and texture with the updated character
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, strlen(text), color);
    texture->texture = SDL_CreateTextureFromSurface(renderer, surface);
    texture->width = surface->w;
    texture->height = surface->h;

    // Free the temporary surface
    SDL_DestroySurface(surface);
}

void Texture_update(Texture* texture, SDL_Renderer* renderer, TTF_Font* font, char* newChars, SDL_Color color) {
    if (strlen(newChars) == 1) {
        // A single character and a null terminator.
        char text[2] = {newChars[0], '\0'};
        createTexture(texture, renderer, font, text, color);
    }
    else {
        createTexture(texture, renderer, font, newChars, color);
    }
}
