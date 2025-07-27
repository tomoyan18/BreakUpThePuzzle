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
#define BALL_SPEED 3.0f

/* ブロック */
#define BLOCK_ROWS 5
#define BLOCK_COLS 8
#define BLOCK_WIDTH 35
#define BLOCK_HEIGHT 15

/* アイテム関連 */
#define MAX_ITEMS 10    //同時に存在できるアイテム数
#define ITEM_SIZE 12    //アイテムの見た目のサイズ
#define MAX_BALLS 5     //最大ボール数

/* ゲームシーン関連 */
typedef enum{
    SCENE_TITLE,
    SCENE_PLAY,
    SCENE_GAMEOVER,
    SCENE_CLEAR
} GameScene;

GameScene currentScene = SCENE_TITLE;

/* アイムの種類 */
typedef enum {
    ITEM_LIFE_PLUS,     //残機+1
    ITEM_PADDLE_EXPAND, //パドルが長くなる
    ITEM_PADDLE_SHRINK, //パドルが短くなる
    ITEM_BALL_PLUS      //ボールが増える
} ItemType;

/* 複数ボール管理用に追加 */
typedef struct {
    float x, y;
    float vx, vy;
    SDL_Rect rect;
    int active;
} Ball;

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
//複数ボール用
Ball balls[MAX_BALLS];

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

/* タイトル画面描画関数 */
void draw_titlle_screen(SDL_Renderer* renderer, TTF_Font* font)
{
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xcc, 255); //明るめの背景
    SDL_RenderClear(renderer);

    draw_text(renderer, font, "Block Breaker", 80, 150);
    draw_text(renderer, font, "Press ENTER to START", 70, 220);
}

/* アイテムの効果適用 */
void apply_item_effect(ItemType type, int* lives, SDL_Rect* paddle)
{
    
    switch (type)
    {
    case ITEM_LIFE_PLUS:
        (*lives)++;
        printf("Got Life Up! Lives = %d\n", *lives);
        break;

    case ITEM_PADDLE_EXPAND:
        printf("Paddle Expanded!\n");
        //パドルを画面端に収まる範囲で長くする（例：+20）
        if(paddle->w < WINDOW_WIDTH - 20)
        {
            paddle->w += 20;
        }
        break;
    
    case ITEM_PADDLE_SHRINK:
        printf("Paddle Shrunk\n");
        //パドルを最小幅まで短くする（例：-20）
        if(paddle->w >40)
        {
            paddle->w -= 20;
        }
        break;
    case ITEM_BALL_PLUS:
        for(int i = 0; i < MAX_BALLS; i++)
        {
            if(!balls[i].active)
            {
                for(int j = 0; j < MAX_BALLS; j++)
                {
                    if(balls[j].active)
                    {
                        balls[i].x = balls[j].x;
                        balls[i].y = balls[j].y;
                        balls[i].vx = balls[j].vx;
                        balls[i].vy = balls[j].vy;
                        balls[i].rect.w = BALL_SIZE;
                        balls[i].rect.h = BALL_SIZE;
                        balls[i].active = 1;
                        printf("Extra Ball! (Not implemented yet)\n");
                        return;
                    }
                }
            }
        }
    
        break;
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
    int ball_waiting = 1;
    int sceneEntered = 0;

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
    for(int i = 0; i < MAX_BALLS; i++)
    {
        balls[i].active = 0;
    }
    balls[0].x = (WINDOW_WIDTH - BALL_SIZE) / 2.0f;
    balls[0].y = paddle.y - BALL_SIZE -2;
    balls[0].vx = BALL_SPEED;
    balls[0].vy = -BALL_SPEED;
    balls[0].rect.w = BALL_SIZE;
    balls[0].rect.h = BALL_SIZE;
    balls[0].active = 1;

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
                
                switch (currentScene)
                {
                    case SCENE_TITLE:
                        if(event.key.keysym.sym == SDLK_RETURN)
                        {
                            //ゲーム初期化
                            score = 0;
                            lives = 3;
                            //ブロック・ボール・アイテム初期化処理をここで呼ぶ
                            currentScene = SCENE_PLAY;
                        }
                        break;
                    
                    case SCENE_PLAY:
                        if(event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            quit_flg = 0;
                        }
                        else if(event.key.keysym.sym == SDLK_LEFT && paddle.x > 0)
                        {
                            paddle.x -= PADDLE_SPEED;
                        }
                        else if(event.key.keysym.sym == SDLK_RIGHT && paddle.x + paddle.w < WINDOW_WIDTH)
                        {
                            paddle.x += PADDLE_SPEED;
                        }
                        else if (event.key.keysym.sym == SDLK_SPACE && ball_waiting)
                        {
                            ball_waiting = 0;
                            float speed = BALL_SPEED;
                            balls[0].vx = 0;
                            balls[0].vy = -speed;
                        }
                        
                        break;

                    case SCENE_GAMEOVER:
                    case SCENE_CLEAR:
                        if (event.key.keysym.sym == SDLK_SPACE)
                        {
                            currentScene = SCENE_TITLE;
                            sceneEntered = 0;

                            score = 0;
                            lives = 3;

                            paddle.x = (WINDOW_WIDTH - PADDLE_WIDTH) / 2;
                            paddle.y = WINDOW_HEIGHT - 50;
                            paddle.w = PADDLE_WIDTH;

                            for(int i = 0; i < MAX_BALLS; i++)
                            {
                                balls[i].active = 0;
                            }
                            balls[0].x = paddle.x + (paddle.w - BALL_SIZE) / 2.0f;
                            balls[0].y = paddle.y - BALL_SIZE - 2;
                            balls[0].vx = BALL_SPEED;
                            balls[0].vy = -BALL_SPEED;
                            balls[0].rect.w = BALL_SIZE;
                            balls[0].rect.h = BALL_SIZE;
                            balls[0].active = 1;
                            
                            for(int i = 0; i < BLOCK_ROWS; i++)
                            {
                                for(int j = 0; j < BLOCK_COLS; j++)
                                {
                                    block_visible[i][j] = 1;
                                }
                            }

                            for(int i = 0; i < MAX_ITEMS; i++)
                            {
                                items[i].active = 0;
                            }

                            ball_waiting = 1;
                            

                        }
                        break;
                }

            }
        }

        if(ball_waiting)
        {
            balls[0].x = paddle.x + (paddle.w - BALL_SIZE) / 2.0f;
            balls[0].y = paddle.y - BALL_SIZE - 2;
            balls[0].rect.x = (int)balls[0].x;
            balls[0].rect.y = (int)balls[0].y;
        }

        /* 複数ボール更新 */
        for(int i = 0; i < MAX_BALLS; i++)
        {
            if(!balls[i].active) continue;
            if(ball_waiting) continue;

            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;
            balls[i].rect.x = (int)balls[i].x;
            balls[i].rect.y = (int)balls[i].y;

            /* 壁に当たったら跳ね返る */
            if(balls[i].rect.x <= 0 || balls[i].rect.x + BALL_SIZE >= WINDOW_WIDTH)
            {
                balls[i].vx *= -1;
            }

            if(balls[i].rect.y <= 0)
            {
                balls[i].vy *= -1;
            }

            /* パドルに当たったら跳ね返る */
            if(SDL_HasIntersection(&balls[i].rect, &paddle) && balls[i].vy > 0)
            {
                //パドル中心とボール中心の差
                int center = paddle.x + paddle.w / 2;
                int ball_center = balls[i].rect.x + BALL_SIZE /2;
                //[-1.0, 1.0]の範囲で正規化
                float norm = (float)(ball_center - center) / (paddle.w / 2);
                //最大角度を調整(30〜60度)
                float angle = norm * (M_PI / 3);

                // 速度ベクトルの大きさを保つ
                float speed = sqrt(balls[i].vx * balls[i].vx + balls[i].vy * balls[i].vy);
                if(speed < 1.5f) speed = 1.5f;
                balls[i].vx = speed * sin(angle);
                balls[i].vy = -speed * cos(angle);
            }

            /* ブロックと衝突 */
            for(int row = 0; row < BLOCK_ROWS; row++)
            for(int col = 0; col < BLOCK_COLS; col++)
            {
                if(block_visible[row][col] && SDL_HasIntersection(&balls[i].rect, &blocks[row][col]))
                {
                    block_visible[row][col] = 0;
                    //balls[i].vy *= -1;
                    score += 10;

                    if(rand() % 5 == 0)
                    {
                        spawn_item(blocks[row][col].x + BLOCK_WIDTH / 2 - ITEM_SIZE / 2,
                                   blocks[row][col].y + BLOCK_HEIGHT,
                                   (ItemType)(rand() % 4));
                    }
                   
                    block_visible[row][col] = 0;
                    score += 10;
                    
                    SDL_Rect* b = &balls[i].rect;
                    SDL_Rect* bl = &blocks[row][col];

                    int ballCenterX = b->x + b->w / 2;
                    int ballCenterY = b->y + b->h / 2;

                    int blockCenterX = bl->x + bl->w / 2;
                    int blockCenterY = bl->y + bl->h / 2;

                    int dx = ballCenterX - blockCenterX;
                    int dy = ballCenterY - blockCenterY;

                    // ブロック幅と高さの半分
                    int halfWidth = bl->w / 2;
                    int halfHeight = bl->h / 2;

                    // ボールの速度ベクトル（元々の速度）
                    float vx = balls[i].vx;
                    float vy = balls[i].vy;

                    // 衝突の重なり幅（距離）を計算（絶対値で）
                    float overlapX = halfWidth - abs(dx);
                    float overlapY = halfHeight - abs(dy);

                    // 最も小さい重なり方向に跳ね返りを変える
                    if(overlapX < overlapY)
                    {
                        // 左右にぶつかったので横方向の速度を反転
                        balls[i].vx = -vx;
                    }
                    else
                    {
                        // 上下にぶつかったので縦方向の速度を反転
                        balls[i].vy = -vy;
                    }

                }
            }

            if(balls[i].rect.y > WINDOW_HEIGHT)
            {
                balls[i].active = 0;
            }
        }

        /* ボールが全て消えたら */
        int active_ball = 0;
        for(int i = 0; i < MAX_BALLS; i++)
        {
            if(balls[i].active) active_ball++;
        }

        if(active_ball == 0)
        {
            lives--;
            if(lives <= 0)
            {
                currentScene = SCENE_GAMEOVER;
                ball_waiting = 1; 
            }
            else
            {
                balls[0].x = (WINDOW_WIDTH - BALL_SIZE) / 2.0f;
                balls[0].y = paddle.y - BALL_SIZE - 2;
                balls[0].vx = BALL_SPEED;
                balls[0].vy = -BALL_SPEED;
                balls[0].active = 1;

                ball_waiting = 1;

                printf("Ball fell. Lives left: %d\n", lives);
                
                SDL_Delay(1000);
            }
        }

        /* アイテム移動と取得判定 */
        for(int i = 0; i < MAX_ITEMS; i++)
        {
            if(items[i].active)
            {
                items[i].y += items[i].vy;
                items[i].rect.x = (int)items[i].x;
                items[i].rect.y = (int)items[i].y;

                if(SDL_HasIntersection(&paddle, &items[i].rect))
                {
                    apply_item_effect(items[i].type, &lives, &paddle);
                    items[i].active = 0;
                }
                else if(items[i].y > WINDOW_HEIGHT)
                    items[i].active = 0;
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
            currentScene = SCENE_CLEAR;
            ball_waiting = 1;
        }

        if((currentScene == SCENE_GAMEOVER || currentScene == SCENE_CLEAR) && !sceneEntered)
        {
            if(currentScene == SCENE_GAMEOVER)
            {
                printf("Game Over. Final Score: %d\n", score);
                sceneEntered = 1;
            } 
            else 
            {
                printf("Game Clear! Final Score: %d\n", score);
                sceneEntered = 1;
            }
        }

        switch (currentScene)
        {
            case SCENE_TITLE:
                //タイトル画面描画
                draw_titlle_screen(renderer, font);
                break;
            case SCENE_PLAY:
                /* 背景塗りつぶし */
                SDL_SetRenderDrawColor(renderer, 0xdf, 0xff, 0xdf, 255);
                SDL_RenderClear(renderer);

                /* パドル描画(黒) */
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &paddle);

                /* ボール描画 */
                for(int i = 0; i < MAX_BALLS; i++)
                {
                    if(balls[i].active)
                    {
                        SDL_RenderFillRect(renderer, &balls[i].rect);
                    }  
                }

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
                for(int i = 0; i < MAX_ITEMS; i++)
                {
                    if(!items[i].active) continue;

                    SDL_Rect r = items[i].rect;
                    switch(items[i].type)
                    {
                        case ITEM_LIFE_PLUS:
                            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                            SDL_RenderDrawLine(renderer, r.x + r.w / 2, r.y, r.x + r.w / 2, r.y + r.h);
                            SDL_RenderDrawLine(renderer, r.x, r.y + r.h / 2, r.x + r.w, r.y + r.h / 2);
                            break;
                        case ITEM_PADDLE_EXPAND:
                            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                            SDL_RenderFillRect(renderer, &r);
                            SDL_RenderDrawLine(renderer, r.x, r.y + r.h / 2, r.x + r.w, r.y + r.h / 2);
                            break;
                        case ITEM_PADDLE_SHRINK:
                            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                            SDL_RenderFillRect(renderer, &r);
                            SDL_RenderDrawLine(renderer, r.x + r.w / 2, r.y, r.x + r.w / 2, r.y + r.h);
                            break;
                        case ITEM_BALL_PLUS:
                            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // オレンジ
                            SDL_RenderFillRect(renderer, &r);
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 黒で丸っぽく描写（代用）
                            SDL_RenderDrawLine(renderer, r.x, r.y, r.x + r.w, r.y + r.h);
                            SDL_RenderDrawLine(renderer, r.x + r.w, r.y, r.x, r.y + r.h);
                            break;
                    }
                }

                /* 残機表示処理  */
                char lives_text[32];
                snprintf(lives_text, sizeof(lives_text), "Lives: %d", lives);    //表示するテキスト
                draw_text(renderer, font, lives_text, WINDOW_WIDTH - 80, 5);    //画面右上に描画

                break;
            
            case SCENE_GAMEOVER:
                //ゲームオーバー画面描画
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                draw_text(renderer, font, "Game Over", 110, 180);
                draw_text(renderer, font, "Press ENTER to Restart", 50, 230);
                break;
            case SCENE_CLEAR:
                // クリア画面描画
                SDL_SetRenderDrawColor(renderer, 0xee, 0xff, 0xee, 255);
                SDL_RenderClear(renderer);
                draw_text(renderer, font, "Stage Cleared!", 90, 180);
                draw_text(renderer, font, "Press ENTER to Return", 60, 230);
                break;
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