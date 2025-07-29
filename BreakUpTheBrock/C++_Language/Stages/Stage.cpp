#include "Stage.h"

std::vector<Block> Stage::createStage(int stageNum)
{
    std::vector<Block> blocks;
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            blocks.push_back(Block(j * 70 + 10, 1 * 30 + 30));
        }
    }

    return blocks;

}