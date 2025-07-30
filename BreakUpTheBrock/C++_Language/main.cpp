#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Game.h"

int main(int argc. char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    Game game;
    game.run();

    TTF_Quit();
    SDL_Quit();
    return 0;
}