#pragma once
#include <SDL2/SDL.h>

class
{
    public:
        Paddle();
        void update();
        void draw(SDL_Renderer* renderer);
        void handleEvent(const SDL_Event& e);
        void expand();
        void shrink();
        void reset();

        int getX() const;
        int getY() const;
        int getWidth() const;
        SDL_Rect getRect() const;

    private:
        int x, y;
        int width, height;
        int speed;
};