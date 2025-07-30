#pragma once
#include <SDL2/SDL.h>

enum ItemType
{
    ITEM_LIFE_PLUS = 1,
    ITEM_PADDLE_LONG,
    ITEM_PADDLE_SHORT,
    ITEM_BALL_PLUS
};

class Item
{
    public:
        Item(int x, int y, ItemType type);
        void update();
        void draw(SDL_Renderer* renderer);
        bool checkCollision(const SDL_Rect& paddle);
        bool isCollected() const;
        void markCollected();
        ItemType getType() const;

    private:
        int x, y;
        ItemType type;
        bool collected;
};