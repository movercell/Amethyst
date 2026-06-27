#pragma once

#include <vector>
#include "engine/master.h"

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

        Block AllocPadded(uint16_t sizex, uint16_t sizey, uint16_t padding) {
            auto ret = Alloc(sizex + padding * 2, sizey + padding * 2);
            return Block(ret.PosX + padding, ret.PosY + padding, sizex, sizey);
        }
        void FreePadded(Block block, uint16_t padding) {
            block.PosX -= padding;
            block.PosY -= padding;
            block.SizeX += padding * 2;
            block.SizeY += padding * 2;
            
            Free(block);
        }
    };

}