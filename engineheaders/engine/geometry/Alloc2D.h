#pragma once

#include <vector>
#include <functional>
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
        uint16_t AlignmentX;
        uint16_t AlignmentY;
        uint16_t PadX;
        uint16_t PadY;

        std::vector<Block> FreeBlocks;

        std::function<void(Block)> AllocCallback;
        std::function<void(Block)> FreeCallback;

        Block RawAlloc(uint16_t sizex, uint16_t sizey);
        void RawFree(Block block);
    public:
        Alloc2D(uint16_t sizex,
                uint16_t sizey,
                uint16_t padx = 0,
                uint16_t pady = 0,
                uint16_t alignmentx = 1,
                uint16_t alignmenty = 1,
                int ReserveSpaceForBlocks = 256) : 
                        SizeX(sizex),
                        SizeY(sizey),
                        AlignmentX(alignmentx),
                        AlignmentY(alignmenty),
                        PadX(padx),
                        PadY(pady) {
            FreeBlocks.reserve(ReserveSpaceForBlocks);

            FreeBlocks.emplace_back(0, 0, SizeX, SizeY);
        }
        ~Alloc2D() {
            if ((FreeBlocks[0].SizeX != SizeX) || (FreeBlocks[0].SizeY != SizeY)) {
                Engine::Error("Alloc2D: Attemped to destruct without freeing all the blocks!");
            }
        }

        Block ENGINEEXPORT Alloc(uint16_t sizex, uint16_t sizey);
        void ENGINEEXPORT Free(Block block);

        //! Sets callback functions.(Sends in the actual full block in it's pure size and position.) 
        void SetCallbacks(decltype(AllocCallback) alloccallback, decltype(FreeCallback) freecallback) {
            AllocCallback = alloccallback;
            FreeCallback = freecallback;
        }
    };

}