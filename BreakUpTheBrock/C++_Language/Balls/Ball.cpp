#include "Balls/Ball.h"

Ball::Ball(const Paddle& paddle)
{
    radius = 8;
    x = paddle.getX() + paddle.getWidth() / 2;
    y = paddle.getY() - radius * 2;
    vx = 4;
    vy = -4;
    launched = false;
}

void Ball::update()
{
    if(!launched) return;

    x += vx;
    y += vy;

    if(x < 0 || x > 640 - radius) vx = -vx;
    if(y < 0) vy = -vy;
}

void Ball::draw(SDL_Renderer* renderer)
{
    SDL_Rect rect = {(int)x, (int)y, radius * 2, radius * 2};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void Ball::handleEvent(const SDL_Event& e, const Paddle& paddle)
{
    if(!launched && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
    {
        launched =true;
    }

    if(!launched)
    {
        x = paddle.getX() + paddle.getWidth() / 2;
        y = paddle.getY() - radius * 2;
    }
}

bool Ball::checkCollision(const Paddle& paddle)
{
    SDL_Rect ballRect = {(int)x, (int)y, radius * 2, radius * 2};
    SDL_Rect paddleRect = paddle.getRect();
    if(SDL_HasIntersection(&ballRect, &paddleRect))
    {
        vy = -vy;
        //追加
        float paddleCenter = paddle.getX() + paddle.getWidth() / 2.0f;
        float ballCenter = x + radius;
        float diff = (ballCenter - paddleCenter) / (paddle.getWidth() / 2.0f); // -1.0 ～ 1.0
        vx = diff * 5.0f; // ← 角度調整（値が大きいと激しく曲がる）
 
        return true;
    }

    return false;
}

bool Ball::checkCollision(Block& block)
{
    if(block.isDestroyed()) return false;
    SDL_Rect ballRect = {(int)x, (int)y, radius * 2, radius * 2};
    SDL_Rect blockRect = block.getRect();
    if(SDL_HasIntersection(&ballRect, &blockRect))
    {
        vy = -vy;
        block.hit();
        return true;
    }

    return false;
}

bool Ball::isOutOfBounds() const
{
    return y > 480;
}