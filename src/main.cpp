#include <iostream>
#include "chip8.h"
#include "SDL2/SDL.h"
#include <thread>
#include <chrono>
int main()
{
    Chip8 chip;

    int w = 1024;
    int h = 512;

    SDL_Window* window = NULL;
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        exit(1);
    }

    window = SDL_CreateWindow(
            "Chip8",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            w, h, SDL_WINDOW_SHOWN
            );


    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            64, 32);

    uint32_t pixels[2048];

    std::string s = "";
    chip.read_rom(s);

    while(true)
    {
        chip.emulate();

        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) exit(0);
        }

        if(chip.draw_flag)
        {
            chip.draw_flag = false;

            for(int i = 0; i < 2048; i++)
            {
                uint8_t pixel = chip.graphics[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }

            SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

    }
    return 0;    
}
