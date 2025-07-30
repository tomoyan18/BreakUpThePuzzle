#include "Stages/Stage.h"

std::vector<Block> Stage::createStage(int stageNum)
{
    std::vector<Block> blocks;

    const int blockWidth = 60;
    const int blockHeight = 20;
    const int cols = 6;   // 列数（横のブロック数）
    const int rows = 4;   // 行数（縦のブロック数）
    const int spacingX = 10; // 横方向の間隔
    const int spacingY = 8;  // 縦方向の間隔
    const int offsetX = 50;  // 左側余白
    const int offsetY = 50;  // 上側余白

    for(int row = 0; row < rows; ++row)
    {
        for(int col = 0; col < cols; ++col)
        {
            int x = offsetX + col * (blockWidth + spacingX);
            int y = offsetY + row * (blockHeight + spacingY);
            blocks.emplace_back(x, y);
        }
    }

    return blocks;
}