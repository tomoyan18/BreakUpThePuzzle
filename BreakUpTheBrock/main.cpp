#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

//画面サイズ
#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240

//パドル関連
#define PADDLE_WIDTH 60
#define PADDLE_HEIGHT 10
#define PADDLE_SPEED 15

//ボール関連
#define BALL_SIZE 8
#define BALL_SPEED 2

/* ブロック */
#define BLOCK_ROWS 5
#define BLOCK_COLS 8
#define BLOCK_WIDTH 35
#define BLOCK_HEIGHT 15


int main(int argc, char* argv[])
{
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    int quit_flg = 1;

    /*初期化*/
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL_Init(): %s\n", SDL_GetError());
        exit(1);
    }

    /* ウィンドウ作成 */
    window = SDL_CreateWindow("Block Breaker",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if(window == NULL)
    {
        fprintf(stderr, "Can not create window\n");
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL)
    {
        fprintf(stderr, "Can not create renderer\n");
        exit(1);
    }

    /* パドル初期位置 */
    SDL_Rect paddle = {
        .x = (WINDOW_WIDTH - PADDLE_WIDTH) /2,
        .y = WINDOW_HEIGHT - 30,
        .w = PADDLE_WIDTH,
        .h = PADDLE_HEIGHT
    };

    /* ボール初期位置と速度 */
    SDL_Rect ball = {
        .x = WINDOW_WIDTH / 2,
        .y = WINDOW_HEIGHT / 2,
        .w = BALL_SIZE,
        .h = BALL_SIZE
    };

    int ball_vx = BALL_SPEED;
    int ball_vy = -BALL_SPEED;
    
    /* ブロック */
    SDL_Rect blocks[BLOCK_ROWS][BLOCK_COLS];
    int block_visible[BLOCK_ROWS][BLOCK_COLS];

    for(int i =0; i < BLOCK_ROWS; i++)
    {
        for(int j = 0; j < BLOCK_COLS; j++)
        {
            blocks[i][j].x = j * (BLOCK_WIDTH + 3) + 5;
            blocks[i][j].y = i * (BLOCK_HEIGHT + 3) + 10;
            blocks[i][j].w = BLOCK_WIDTH;
            blocks[i][j].h = BLOCK_HEIGHT;
            block_visible[i][j] = 1;
        }
    }

    /* ゲームループ */
    while (quit_flg)
    {
        //入力処理
        while (SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                quit_flg = 0;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit_flg = 0;
                        break;
                    case SDLK_LEFT:
                        if(paddle.x > 0)
                        {
                            paddle.x -= PADDLE_SPEED;
                        }
                        break;
                    case SDLK_RIGHT:
                        if(paddle.x + PADDLE_WIDTH < WINDOW_WIDTH)
                        {
                            paddle.x += PADDLE_SPEED;
                        }
                        break;
                }
            }
        }
        /* ボール移動 */
        ball.x += ball_vx;
        ball.y += ball_vy;

        /* 壁に当たったら跳ね返る */
        if(ball.x <= 0 || ball.x + BALL_SIZE >= WINDOW_WIDTH)
        {
            ball_vx *= -1;
        }
        if(ball.y <= 0)
        {
            ball_vy *= -1;
        }

        /* パドルに当たったら跳ね返る */
        if(SDL_HasIntersection(&ball, &paddle) && ball_vy > 0)
        {
            ball_vy *= -1;
        }

        /* ブロックと衝突 */
        for(int i = 0; i < BLOCK_ROWS; i++)
        {
            for(int j = 0; j < BLOCK_COLS; j++)
            {
                if(block_visible[i][j] && SDL_HasIntersection(&ball, &blocks[i][j]))
                {
                    block_visible[i][j] = 0;
                    ball_vy *= -1;
                    goto skip_check;
                }
            }
        }
        skip_check:;

        /* 下に落ちたらゲーム終了 */
        if(ball.y > WINDOW_HEIGHT)
        {
            printf("Ball fell. Game Over\n");
            quit_flg = 0;
        }

        /* 背景塗りつぶし */
        SDL_SetRenderDrawColor(renderer, 0xdf, 0xff, 0xdf, 255);
        SDL_RenderClear(renderer);

        /* パドル描画(黒) */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &paddle);

        /* ボール描画 */
        SDL_RenderFillRect(renderer, &ball);

        /* ブロック描画 */
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for(int i = 0; i < BLOCK_ROWS; i++)
        {
            for(int j = 0; j < BLOCK_COLS; j++)
            {
                if(block_visible[i][j])
                {
                    SDL_RenderFillRect(renderer, &blocks[i][j]);
                }
            }
        }

        /* 描画反映 */
        SDL_RenderPresent(renderer);

        SDL_Delay(16); //約60FPS
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
    
}