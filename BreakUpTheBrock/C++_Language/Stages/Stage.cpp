#include "Stages/Stage.h"

std::vector<Block> Stage::createStage(int stageNum)
{
    std::vector<Block> blocks;

    const int blockWidth = 60;
    const int blockHeight = 20;
    //const int cols = 6;   // 列数（横のブロック数）
    //const int rows = 4;   // 行数（縦のブロック数）
    const int spacingX = 10; // 横方向の間隔
    const int spacingY = 8;  // 縦方向の間隔
    //const int offsetX = 50;  // 左側余白
    const int offsetY = 50;  // 上側余白

    int cols = 0;
    int rows = 0;

    switch ((stageNum))
    {
        case 1: //ステージ0: 6列✖️4行
            cols = 6;
            rows = 4;
            break;
        case 2: //ステージ1：　８列✖️３行
            cols = 8;
            rows = 3;
            break;
        case 3: //ステージ２：　４列✖️６行
            cols = 4;
            rows = 6;
            break;
        default:
            cols = 6;
            rows = 4;
            break;
    }

    //💡 中央寄せのために offsetX を列数に応じて再計算
    int totalWidth = cols * blockWidth + (cols - 1) * spacingX;
    int offsetX = (640 - totalWidth) / 2; // 画面幅640に対して中央揃え


    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            int x = offsetX + col * (blockWidth + spacingX);
            int y = offsetY + row * (blockHeight + spacingY);
            blocks.emplace_back(x, y);
        }
    }

    return blocks;
}