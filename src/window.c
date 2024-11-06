#include "window.h"

int Window_init(Window* window) {
    window->width = 800;
    window->height = 600;
    window->window = SDL_CreateWindow("Typing Trainer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window->width, window->height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window->window) return -1;

    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED);
    if (!window->renderer)
    {
        SDL_DestroyWindow(window->window);
        return -1;
    }
    return 0;
}

void Window_destroy(Window* window) {
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
}

void Window_resize(Window* window, SDL_Event e) {
    if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
        window->width = e.window.data1;
        window->height = e.window.data2;
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

void Window_fillRect(Window* win, SDL_Rect r) {
    SDL_RenderFillRect(win->renderer, &r);
}
