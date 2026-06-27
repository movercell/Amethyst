#pragma once

#include <vector>
#include "master.h"

namespace Geometry {

    struct Alloc2D {
        struct Block {
            uint16_t PosX = 0;
            uint16_t PosY = 0;
            uint16_t SizeX = 0;
            uint16_t SizeY = 0;

            Block(uint16_t posx, uint16_t posy, uint16_t sizex, uint16_t sizey) : PosX(posx), PosY(posy), SizeX(sizex), SizeY(sizey) {}
            Block() {}
        };
    private:
        uint16_t SizeX;
        uint16_t SizeY;

        std::vector<Block> FreeBlocks;
    public:
        Alloc2D(uint16_t sizex, uint16_t sizey,int ReserveSpaceForBlocks = 256) : SizeX(sizex), SizeY(sizey) {
            FreeBlocks.reserve(ReserveSpaceForBlocks);

            FreeBlocks.emplace_back(0, 0, SizeX, SizeY);
        }

        Block ENGINEEXPORT Alloc(uint16_t sizex, uint16_t sizey);
        void ENGINEEXPORT Free(Block block);
    };

}