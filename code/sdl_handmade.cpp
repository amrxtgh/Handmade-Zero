#include <SDL3/SDL.h>
#include <cstdint>

static bool GlobalRunning = true;

int main(int, char *[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* Window = SDL_CreateWindow(
        "Handmade Zero",
        1280,
        720,
        0
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
                case SDL_EVENT_QUIT: {
                    SDL_Log("SDL_Quit received.");
                    GlobalRunning = false;
                } break;
                case SDL_EVENT_KEY_DOWN: {
                    if (Event.key.key == SDLK_ESCAPE) {
                        GlobalRunning = false;
                    }
                } break;
            }
        }
        ColorOffset += 2;
        uint32_t Color = SDL_MapSurfaceRGB(WindowSurface, 0, 0, ColorOffset);
        SDL_FillSurfaceRect(WindowSurface, NULL, Color);
        SDL_UpdateWindowSurface(Window);
    }

    SDL_DestroyWindow(Window);
    SDL_Quit();
    return 0;
}
