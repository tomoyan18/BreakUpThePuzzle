#include "Paddles/Paddle.h"

Paddle::Paddle()
{
    reset();
}

void Paddle::reset()
{
    x = 320 - 50;
    y = 450;
    width = 100;
    height = 10;
    speed = 8;
}

void Paddle::update()
{
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_LEFT]) x -= speed;
    if(state[SDL_SCANCODE_RIGHT]) x += speed;

    if(x < 0) x = 0;
    if(x + width > 640) x = 640 - width;
}

void Paddle::draw(SDL_Renderer* renderer)
{
    SDL_Rect rect = { x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void Paddle::handleEvent(const SDL_Event& e)
{
    //今は未実装
}
void Paddle:: expand()
{
    width = 140;
}

void Paddle::shrink()
{
    width = 60;
}

int Paddle::getX() const { return x; }
int Paddle::getY() const { return y; }
int Paddle::getWidth() const { return width; } 

SDL_Rect Paddle::getRect() const {
    return {x, y, width, height};
}