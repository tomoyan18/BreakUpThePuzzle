#include "Games/Game.h"
#include <SDL2/SDL_image.h>
#include <ctime>

Game::Game()
{
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
    font = TTF_OpenFont("Assets/Fonts/arial.ttf", 24);
    srand((unsigned int)time(NULL));
    state = GameState::TITLE;
    lives = 3;
    score = 0;
    stageNum = 0;
    resetStage();
}

Game::~Game()
{
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Game::run()
{
    bool running = true;
    while (running)
    {
        handleEvents(running);
        update();
        draw();
        SDL_Delay(16);
    }
}

void Game::handleEvents(bool& running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;
        if (state == GameState::TITLE && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
            state = GameState::PLAY;
        } else if ((state == GameState::GAMEOVER || state == GameState::CLEAR) && e.type == SDL_KEYDOWN) {
            state = GameState::TITLE;
            lives = 3;
            score = 0;
            stageNum = 0;
            resetStage();
        } else if (state == GameState::PLAY) {
            paddle.handleEvent(e);
            for (auto& ball : balls) ball.handleEvent(e, paddle);
        }
    }
}

void Game::update() {
    switch (state) {
        case GameState::TITLE: updateTitle(); break;
        case GameState::PLAY: updatePlay(); break;
        case GameState::GAMEOVER: updateGameOver(); break;
        case GameState::CLEAR: updateClear(); break;
    }
}

void Game::draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    switch (state) {
        case GameState::TITLE:
            drawText("Press SPACE to Start", 180, 220, {255, 255, 255});
            break;
        case GameState::PLAY:
            paddle.draw(renderer);
            for (auto& ball : balls) ball.draw(renderer);
            for (auto& block : blocks) if (!block.isDestroyed()) block.draw(renderer);
            for (auto& item : items) item.draw(renderer);
            drawText(("Lives: " + std::to_string(lives)).c_str(), 10, 10, {255, 255, 255});
            drawText(("Score: " + std::to_string(score)).c_str(), 520, 10, {255, 255, 255});
            break;
        case GameState::GAMEOVER:
            drawText("GAME OVER", 230, 200, {255, 0, 0});
            drawText("Press any key to return", 170, 250, {255, 255, 255});
            break;
        case GameState::CLEAR:
            drawText("CLEAR!", 250, 200, {0, 255, 0});
            drawText("Press any key to return", 170, 250, {255, 255, 255});
            break;
    }

    SDL_RenderPresent(renderer);
}

void Game::resetStage() {
    balls.clear();
    items.clear();
    paddle.reset();
    balls.push_back(Ball(paddle));
    blocks = Stage::createStage(stageNum);
}

void Game::updateTitle() {}
void Game::updateGameOver() {}
void Game::updateClear() {}

void Game::updatePlay() {
    paddle.update();
    for (auto& ball : balls) ball.update();

    for (auto& ball : balls) {
        ball.checkCollision(paddle);
        for (auto& block : blocks) {
            if (!block.isDestroyed() && ball.checkCollision(block)) {
                block.hit();
                score += 10;
                if (rand() % 5 == 0) spawnItem(block.getX(), block.getY());
                break;
            }
        }
    }

    for (auto& item : items) {
        item.update();
        if (item.checkCollision(paddle.getRect())) {
            switch (item.getType()) {
                case ITEM_LIFE_PLUS: lives++; break;
                case ITEM_PADDLE_LONG: paddle.expand(); break;
                case ITEM_PADDLE_SHORT: paddle.shrink(); break;
                case ITEM_BALL_PLUS: balls.push_back(Ball(paddle)); break;
                default: break;
            }
            item.markCollected();
        }
    }

    items.erase(std::remove_if(items.begin(), items.end(),
        [](const Item& item) { return item.isCollected(); }),
        items.end());

    balls.erase(std::remove_if(balls.begin(), balls.end(),
        [](const Ball& ball) { return ball.isOutOfBounds(); }),
        balls.end());

    if (balls.empty()) {
        lives--;
        if (lives <= 0) state = GameState::GAMEOVER;
        else resetStage();
    }

    bool allCleared = true;
    for (auto& block : blocks) if (!block.isDestroyed()) allCleared = false;
    if (allCleared) state = GameState::CLEAR;
}

void Game::drawText(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::spawnItem(int x, int y) {
    ItemType type = static_cast<ItemType>(rand() % 4 + 1);
    items.push_back(Item(x, y, type));
}