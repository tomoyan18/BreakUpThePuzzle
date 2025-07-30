#include "Blocks/Block.h"

Block::Block(int x_, int y_) : x(x_), y(y_), destroyed(false)
{

}

void Block::draw(SDL_Renderer* renderer)
{
    if(destroyed) return;
    SDL_Rect rect = {x, y, 60, 20};
    SDL_SetRenderDrawColor(renderer, 255, 200, 100, 255);
    SDL_RenderFillRect(renderer, &rect);
}

bool Block::isDestroyed() const
{
    return destroyed;
}

void Block::hit()
{
    destroyed = true;
}

SDL_Rect Block::getRect() const
{
    return { x, y, 60, 20 };
}

int Block::getX() const{ return x; }
int Block::getY() const{ return y; }