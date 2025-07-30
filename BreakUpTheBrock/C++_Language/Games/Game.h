#pragma once
#include <vector>
#include <SDL2/SDL_ttf.h>
#include "Balls/Ball.h"
#include "Paddles/Paddle.h"
#include "Blocks/Block.h"
#include "Items/Item.h"
#include "Stages/Stage.h"

enum class GameState
{
    TITLE,
    PLAY,
    GAMEOVER,
    CLEAR
};

class Game
{
    public:
        Game();
        ~Game();
        void run();

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        TTF_Font* font;

        GameState state;
        int lives;
        int score;
        int stageNum;

        Paddle paddle;
        std::vector<Ball> balls;
        std::vector<Block> blocks;
        std::vector<Item> items;

        void init();
        void handleEvents(bool& running);
        void update();
        void draw();
        void resetStage();
        void drawText(const char* text, int x, int y, SDL_Color color);
        void spawnItem(int x, int y);

        void updateTitle();
        void updatePlay();
        void updateGameOver();
        void updateClear();
};

