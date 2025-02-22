#include "window.h"

int Window_init(Window* window) {
    window->width = 1000;
    window->height = 1000;
    window->window = SDL_CreateWindow("Typing Trainer",  window->width, window->height,  SDL_WINDOW_RESIZABLE);

    if (!window->window) return -1;

    window->renderer = SDL_CreateRenderer(window->window, NULL);
    if (!window->renderer) {
        SDL_DestroyWindow(window->window);
        return -1;
    }
    window->tooSmall = false;
    return 0;
}

void Window_destroy(Window* window) {
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
}

void Window_resize(Window* window, SDL_Event e) {
    if (e.type == SDL_EVENT_WINDOW_RESIZED) {
        int w = e.window.data1;
        int h = e.window.data2;

        if (w < 400 || h < 800) {
            window->tooSmall = true;
        }
        else {
            window->tooSmall = false;
            window->width = e.window.data1;
            window->height = e.window.data2;
        }
    }
}

void Window_setColor(Window* win, SDL_Color color) {
    SDL_SetRenderDrawColor(win->renderer, color.r, color.g, color.b, color.a);
}

void Window_clear(Window* win) {
    SDL_RenderClear(win->renderer);
}

void Window_render(Window* win) {
    SDL_RenderPresent(win->renderer);
}

