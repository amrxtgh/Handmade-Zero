#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstddef>
#include <cstdint>
#include <stdlib.h>


static bool GlobalRunning = true;

int main(int, char *[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* Window = SDL_CreateWindow(
        "Handmade Zero",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1280,
        720,
        SDL_WINDOW_SHOWN
    );
    if (!Window) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Surface* WindowSurface = SDL_GetWindowSurface(Window);
    uint8_t ColorOffset = 0;

    while (GlobalRunning) {
        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            switch (Event.type) {
                case SDL_QUIT: {
                    SDL_Log("SDL_Quit received.");
                    GlobalRunning = false;
                } break;
                case SDL_KEYDOWN: {
                    if (Event.key.keysym.sym == SDLK_ESCAPE) {
                        GlobalRunning = false;
                    }
                } break;
            }
        }
    ColorOffset += 2;
    uint32_t Color = SDL_MapRGB(WindowSurface->format, ColorOffset, 0, ColorOffset);
    SDL_FillRect(WindowSurface, NULL, Color);
    SDL_UpdateWindowSurface(Window);
    }
    SDL_DestroyWindow(Window);
    SDL_Quit();
    return 0;
}
