#pragma once
#include <SDL2/SDL.h>

class Block
{
    public :
        Block(int x, int y);
        void draw(SDL_Renderer* renderer);
        bool isDestroyed() const;
        void hit();
        SDL_Rect getRect() const;
        int getX() const;
        int getY() const;

    private:
        int x, y;
        bool destroyed;
};