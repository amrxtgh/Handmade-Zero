#include <SDL3/SDL.h>
#include <cstdint>
#include <cstring>
#include <sys/mman.h>


#define internal        static
#define local_persist   static
#define global_variable static

global_variable bool GlobalRunning = true;

struct sdl_window_buffer {
    void *Memory;
    int Width;
    int Height;
    int BytesPerPixel;
    int Pitch;
};

global_variable sdl_window_buffer GlobalBackBuffer;
global_variable SDL_Texture* GlobalTexture = nullptr;

internal void ResizeBackbuffer(SDL_Renderer *Renderer, sdl_window_buffer *Buffer, int Width, int Height) {
    if (Buffer->Memory) {
        munmap(Buffer->Memory, Buffer->Width * Buffer->Height * Buffer->BytesPerPixel);
    }
    if (GlobalTexture) {
        SDL_DestroyTexture(GlobalTexture);
        GlobalTexture = nullptr;
    }
    
    // Set Dimensions 
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;

    // Allocating the raw memory
    int BufferSize = Buffer->Width * Buffer->Height * Buffer->BytesPerPixel;
    Buffer->Memory = mmap(0, BufferSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    GlobalTexture = SDL_CreateTexture(
        Renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        Buffer->Width,
        Buffer->Height
    );
}

internal void RenderWeirdGradient(sdl_window_buffer *Buffer, int BlueOffset, int GreenOffset) {
    uint8_t *Row = (uint8_t *)Buffer->Memory;
    for (int Y = 0; Y < Buffer->Height; ++Y) {
        uint32_t *Pixel = (uint32_t *)Row;
        for (int X = 0; X < Buffer->Width; ++X) {
            uint8_t Blue = (X + BlueOffset);
            uint8_t Green = (X + GreenOffset);
            uint8_t Red = 0;
            uint8_t Alpha = 255;

            *Pixel++ = ((Alpha << 24) | (Red << 16) | (Green << 8) | Blue);
        }
        Row += Buffer->Pitch;
    }
}

int main(int, char *[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return 1;
    }

    // SDL_Window *Window = SDL_CreateWindow("Handmade Zero", 1280, 720, 0);
    
    SDL_PropertiesID Props = SDL_CreateProperties();
    SDL_SetStringProperty(Props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Handmade Zero");
    SDL_SetNumberProperty(Props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 1280);
    SDL_SetNumberProperty(Props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 720);
    SDL_SetBooleanProperty(Props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_Window* Window = SDL_CreateWindowWithProperties(Props);
    SDL_DestroyProperties(Props);

    if (!Window) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* Renderer = SDL_CreateRenderer(Window, NULL);
    if (!Renderer) {
        SDL_Log("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(Window);
        SDL_Quit();
        return 1;
    }

    ResizeBackbuffer(Renderer, &GlobalBackBuffer, 1280, 720);

    int XOffset = 0;
    int YOffset = 0;

    while (GlobalRunning) {
        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_EVENT_QUIT) {
                GlobalRunning = false;
            } else if (Event.type == SDL_EVENT_KEY_DOWN) {
                if (Event.key.key == SDLK_ESCAPE) {
                    GlobalRunning = false;
                }
            }
        }
        RenderWeirdGradient(&GlobalBackBuffer, XOffset, YOffset);
        XOffset += 1;
        YOffset += 2;

        SDL_UpdateTexture(GlobalTexture, NULL, GlobalBackBuffer.Memory, GlobalBackBuffer.Pitch);

        // memcpy(WindowSurface->pixels, GlobalBackBuffer.Memory,
        //     GlobalBackBuffer.Width * GlobalBackBuffer.Height * GlobalBackBuffer.BytesPerPixel);

        SDL_RenderClear(Renderer);
        SDL_RenderTexture(Renderer, GlobalTexture, NULL, NULL);
        SDL_RenderPresent(Renderer);
    }
    if (GlobalBackBuffer.Memory) {
        munmap(GlobalBackBuffer.Memory, GlobalBackBuffer.Width * GlobalBackBuffer.Height * GlobalBackBuffer.BytesPerPixel);
    }
    if (GlobalTexture) SDL_DestroyTexture(GlobalTexture);
    SDL_DestroyRenderer(Renderer);
    SDL_Quit();
    return 0;
}
