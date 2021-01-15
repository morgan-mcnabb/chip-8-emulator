#include <iostream>
#include "chip8.h"
#include "SDL2/SDL.h"
int main()
{
    Chip8 chip;
    std::string s = "";

    SDL_Window* window = NULL;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "oh no" << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 512, SDL_WINDOW_SHOWN);

    if(window == NULL)
    {
        std::cout << "oh no again" << std::endl;
        exit(2);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, 1024, 512);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    uint32_t pixels[2048];

    chip.read_rom(s);

    while(true)
    {
        chip.emulate();
        
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
                exit(0);
        }
        if(chip.draw_flag)
        {
            for(int i = 0; i < 2048; i++)
            {
                unsigned char pixel = chip.graphics[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }

            SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(Uint32));

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }
    return 0;    
}
