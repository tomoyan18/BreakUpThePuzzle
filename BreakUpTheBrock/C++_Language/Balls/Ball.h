#pragma once
#include <SDL2/SDL.h>
#include "Paddles/Paddle.h"
#include "Blocks/Block.h"

class Ball
{
    public:
        Ball(const Paddle& paddle);
        void update();
        void draw(SDL_Renderer* renderer);
        void handleEvent(const SDL_Event& e, const Paddle& paddle);
        bool checkCollision(const Paddle& paddle);
        bool checkCollision(Block& block);
        bool isOutOfBounds() const;

    private:
        float x, y;
        float vx, vy;
        int radius;
        bool launched;
};
