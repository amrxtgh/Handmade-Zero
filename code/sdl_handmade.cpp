#include <SDL3/SDL.h>
#include <cstdint>
#include <cstring>
#include <sys/mman.h>

static bool GlobalRunning = true;

struct sdl_window_buffer {
    void *Memory;
    int Width;
    int Height;
    int BytesPerPixel;
    int Pitch;
};

static sdl_window_buffer GlobalBackBuffer;

static void ResizeBackbuffer(sdl_window_buffer *Buffer, int Width, int Height) {
    if (Buffer->Memory && Buffer->Memory != MAP_FAILED) {
        munmap(Buffer->Memory, Buffer->Width * Buffer->Height * Buffer->BytesPerPixel);
    }
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    int BufferSize = Buffer->Width * Buffer->Height * Buffer->BytesPerPixel;
    Buffer->Memory = mmap(0, BufferSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
}

static void RenderWeirdGradient(sdl_window_buffer *Buffer, int BlueOffset, int GreenOffset) {
    uint8_t *Row = (uint8_t *)Buffer->Memory;
    for (int Y = 0; Y < Buffer->Height; ++Y) {
        uint32_t *Pixel = (uint32_t *)Row;
        for (int X = 0; X < Buffer->Width; ++X) {
            uint8_t Blue = (X + BlueOffset);
            uint8_t Green = (X + GreenOffset);
            uint8_t Red = 0;
            *Pixel++ = ((Red << 16) | (Green << 8) | Blue);
        }
        Row += Buffer->Pitch;
    }
}

int main(int, char *[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return 1;
    }
    ResizeBackbuffer(&GlobalBackBuffer, 1280, 720);
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
    SDL_Surface* WindowSurface = SDL_GetWindowSurface(Window);

    int XOffset = 0;
    int YOffset = 0;

    while (GlobalRunning) {
        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_EVENT_QUIT) {
                GlobalRunning = false;
            } else if (Event.type == SDL_EVENT_WINDOW_RESIZED) {
                WindowSurface = SDL_GetWindowSurface(Window);
                ResizeBackbuffer(&GlobalBackBuffer, WindowSurface->w, WindowSurface->h);
            }
        }
        RenderWeirdGradient(&GlobalBackBuffer, XOffset, YOffset);
        XOffset += 1;
        YOffset += 2;

        memcpy(WindowSurface->pixels, GlobalBackBuffer.Memory,
            GlobalBackBuffer.Width * GlobalBackBuffer.Height * GlobalBackBuffer.BytesPerPixel);
        SDL_UpdateWindowSurface(Window);
    }

    if (GlobalBackBuffer.Memory && GlobalBackBuffer.Memory != MAP_FAILED) {
        munmap(GlobalBackBuffer.Memory, GlobalBackBuffer.Width * GlobalBackBuffer.Height * GlobalBackBuffer.BytesPerPixel);
    }
    SDL_DestroyWindow(Window);
    SDL_Quit();
    return 0;
}
