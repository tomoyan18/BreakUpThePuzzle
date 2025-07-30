#pragma once
#include <vector>
#include "Blocks/Block.h"

class Stage
{
    public:
        static std::vector<Block> createStage(int stageNum);
};