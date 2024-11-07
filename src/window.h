#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

#include <stdbool.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width;
    int height;
    bool tooSmall;
} Window;

// Initialization and destruction.
int Window_init(Window* window);
void Window_destroy(Window* window);
void Window_resize(Window* window, SDL_Event e);

// Rendering functions.
void Window_clear(Window* win);
void Window_render(Window* win);
void Window_setColor(Window* win, SDL_Color color);

#endif
