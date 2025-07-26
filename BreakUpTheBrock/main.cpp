#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//画面サイズ
#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 480

//パドル関連
#define PADDLE_WIDTH 60
#define PADDLE_HEIGHT 10
#define PADDLE_SPEED 15

//ボール関連
#define BALL_SIZE 8
#define BALL_SPEED 2.0f

/* ブロック */
#define BLOCK_ROWS 5
#define BLOCK_COLS 8
#define BLOCK_WIDTH 35
#define BLOCK_HEIGHT 15

/* アイテム関連 */
#define MAX_ITEMS 10    //同時に存在できるアイテム数
#define ITEM_SIZE 12    //アイテムの見た目のサイズ

/* アイムの種類 */
typedef enum {
    ITEM_LIFE_PLUS
} ItemType;

/* アイテム構造体 */
typedef struct 
{
    float x, y;
    float vy;
    ItemType type;
    int active;
    SDL_Rect rect;
} Item;

//アイテム管理用配列
Item items[MAX_ITEMS];

/* テキスト描画関数(残機を画面に表示する) */
void draw_text(SDL_Renderer* renderer, TTF_Font* font, 
               const char* text, int x, int y)
{
    SDL_Color color = {0, 0, 0};    //黒色
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);     //描画用サーフェス作成
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); //テクスチャ化
    SDL_Rect dst = {x, y, surface->w, surface->h};  //表示位置とサイズ
    SDL_RenderCopy(renderer, texture, NULL, &dst);  //描画
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

/* アイテムの効果適用 */
void apply_item_effect(ItemType type, int* lives)
{
    if(type == ITEM_LIFE_PLUS)
    {
        (*lives)++;
        printf("Got Life Up! Lives = %d\n", *lives);
    }
}

/* アイテム生成 */
void spawn_item(float x, float y, ItemType type)
{
    for(int i = 0; i < MAX_ITEMS; i++)
    {
        if(!items[i].active)
        {
            items[i].x = x;
            items[i].y = y;
            items[i].vy = 2.0f;
            items[i].type = type;
            items[i].active = 1;
            items[i].rect.w = ITEM_SIZE;
            items[i].rect.h = ITEM_SIZE;
            return;
        }
    }
}


int main(int argc, char* argv[])
{
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    int quit_flg = 1;

    //スコア初期化
    int score = 0;
    //残機の初期値
    int lives = 3;

    //ランダム初期化
    srand((unsigned int)time(NULL));

    /*初期化*/
    if(SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0)
    {
        fprintf(stderr, "SDL_Init or TTF_Init Error: %s\n", SDL_GetError());
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

    /* レンダラー作成 */
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL)
    {
        fprintf(stderr, "Can not create renderer\n");
        exit(1);
    }

    /* フォントを読み込む */
    TTF_Font* font = TTF_OpenFont("Arial.ttf", 16);
    if(!font)
    {
        fprintf(stderr, "Font Load Error: %s\n", TTF_GetError());
        return 1;
    }

    /* パドル初期位置 */
    SDL_Rect paddle = {
        .x = (WINDOW_WIDTH - PADDLE_WIDTH) /2,
        .y = WINDOW_HEIGHT - 50,
        .w = PADDLE_WIDTH,
        .h = PADDLE_HEIGHT
    };

    /* ボール初期位置と速度 */
    float ball_x = (WINDOW_WIDTH - BALL_SIZE) / 2.0f;
    float ball_y = paddle.y - BALL_SIZE -2;
    float ball_vx = BALL_SPEED;
    float ball_vy = -BALL_SPEED;


    SDL_Rect ball = {
        .x = (int)ball_x,
        .y = (int)ball_y, //パドルのすぐ上に配置
        .w = BALL_SIZE,
        .h = BALL_SIZE
    };
    
    /* ブロック */
    SDL_Rect blocks[BLOCK_ROWS][BLOCK_COLS];
    int block_visible[BLOCK_ROWS][BLOCK_COLS];

    for(int i =0; i < BLOCK_ROWS; i++)
    {
        for(int j = 0; j < BLOCK_COLS; j++)
        {
            blocks[i][j].x = j * (BLOCK_WIDTH + 3) + 5;
            blocks[i][j].y = i * (BLOCK_HEIGHT + 5) + 30;
            blocks[i][j].w = BLOCK_WIDTH;
            blocks[i][j].h = BLOCK_HEIGHT;
            block_visible[i][j] = 1;
        }
    }

    /* アイテム配列初期化 */
    for(int i = 0; i < MAX_ITEMS; i++)
    {
        items[i].active = 0;
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
        ball_x += ball_vx;
        ball_y += ball_vy;

        /* 描画用に整数へ */
        ball.x = (int)ball_x;
        ball.y = (int)ball_y;

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
            //パドル中心とボール中心の差
            int paddle_center = paddle.x + paddle.w / 2;
            int ball_center = ball.x + ball.w / 2;
            int diff = ball_center - paddle_center;

           //[-1.0, 1.0]の範囲で正規化
            float norm = (float)diff / (PADDLE_WIDTH / 2);

            //最大角度を調整(30〜60度)
            float angle = norm * (M_PI / 3); //最大60度まで

            float speed = sqrt(ball_vx * ball_vx 
                + ball_vy * ball_vy); // 速度ベクトルの大きさを保つ

            if(speed < 1.5f)
            {
                speed = 1.5f;
            }

            ball_vx = speed * sin(angle);
            ball_vy = -speed * cos(angle);
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
                    score += 10;

                    if(rand() % 3 == 0)
                    {
                        spawn_item(blocks[i][j].x + BLOCK_WIDTH / 2 - ITEM_SIZE / 2,
                                   blocks[i][j].y + BLOCK_HEIGHT, ITEM_LIFE_PLUS);
                    }

                    goto skip_check;
                }
            }
        }
        skip_check:;

        /* アイテム移動と取得判定 */
        for(int i = 0; i < MAX_ITEMS; i++)
        {
            if(items[i].active)
            {
                items[i].y += items[i].vy;
                items[i].rect.x = (int)items[i].x;
                items[i].rect.y = (int)items[i].y;

                //パドルとアイテムの当たり判定
                if(SDL_HasIntersection(&paddle, &items[i].rect))
                {
                    apply_item_effect(items[i].type, &lives);
                    items[i].active = 0;    //アイテム消去
                }
                else if(items[i].y > WINDOW_HEIGHT) //画面外に落ちたら消去
                {
                    items[i].active = 0;
                }
            }
        }

        /* ステージクリア判定 */
        int all_cleared = 1;
        for(int i = 0; i < BLOCK_ROWS; i++)
        {
            for(int j = 0; j < BLOCK_COLS; j++)
            {
                if(block_visible[i][j])
                {
                    all_cleared = 0;
                    break;
                }
            }

            if(!all_cleared) break;

        }

        if(all_cleared)
        {
            /* クリアメッセージとスコア表示 */
            printf("Stage Cleared! Final Score: %d\n", score);
            quit_flg = 0;
        }

        /* 下に落ちたらゲーム終了 */
        if(ball.y > WINDOW_HEIGHT)
        {
            lives--;
            if(lives <= 0)
            {
                /* ゲームオーバーでもスコア表示 */
                printf("Ball fell. Game Over. Final Score: %d\n", score);
                quit_flg = 0;
            }
            else
            {
                //ボール位置リセット
                ball_x = (WINDOW_WIDTH - BALL_SIZE) / 2.0f;
                ball_y = paddle.y - BALL_SIZE - 2;
                ball_vx = BALL_SPEED;
                ball_vy = -BALL_SPEED;
                printf("Ball fell. Lives left: %d\n", lives);
                SDL_Delay(1000);
            }
            
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

        /* アイテム描画(赤で表示) */
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for(int i = 0; i < MAX_ITEMS; i++)
        {
            if(items[i].active)
            {
                SDL_RenderFillRect(renderer, &items[i].rect);
            }
        }

        /* 残機表示処理  */
        char lives_text[32];
        snprintf(lives_text, sizeof(lives_text), "Lives: %d", lives);    //表示するテキスト
        draw_text(renderer, font, lives_text, WINDOW_WIDTH - 80, 5);    //画面右上に描画

        /* 描画反映 */
        SDL_RenderPresent(renderer);

        SDL_Delay(16); //約60FPS
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
    
}