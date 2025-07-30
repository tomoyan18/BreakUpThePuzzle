#include "Items/Item.h"

Item::Item(int x_, int y_, ItemType type_) : 
    x(x_), y(y_), type(type_), collected(false)
{

}

void Item::update()
{
    y += 3;
    if(y > 480) collected = true;
}

void Item::draw(SDL_Renderer* renderer)
{
    if(collected) return;
    SDL_Rect rect = {x, y, 16, 16};

    switch (type)
    {
        case ITEM_LIFE_PLUS:
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            break;
        case ITEM_PADDLE_LONG:
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            break;
        case ITEM_PADDLE_SHORT:
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            break;
        case ITEM_BALL_PLUS:
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            break;
    }

    SDL_RenderFillRect(renderer, &rect);

}

bool Item::checkCollision(const SDL_Rect& paddle)
{
    SDL_Rect itemRect = {x, y, 16, 16};
    if(SDL_HasIntersection(&itemRect, &paddle))
    {
        collected = true;
        return true;
    }

    return false;

}

bool Item::isCollected() const
{
    return collected;
}

void Item::markCollected()
{
    collected = true;
}

ItemType Item::getType() const
{
    return type;
}