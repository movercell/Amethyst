#include "engine/geometry/Alloc2D.h"

Geometry::Alloc2D::Block Geometry::Alloc2D::Alloc(uint16_t sizex, uint16_t sizey) {
    int BestFitIndex = -1;
    int BestFitScore = std::numeric_limits<int>::max(); // The lower, the better.
    Block ret;
    int size = sizex * sizey;

    // Scoring logic.
    for (int block = 0; block < FreeBlocks.size(); block++) {
        if ((FreeBlocks[block].SizeX == sizex) && (FreeBlocks[block].SizeY == sizey)) {
            // Perfect fit, nice!
            ret = FreeBlocks[block];
            std::swap(FreeBlocks[block], FreeBlocks.back());
            FreeBlocks.pop_back();
            return ret;
        }

        if ((FreeBlocks[block].SizeX == sizex) || (FreeBlocks[block].SizeY == sizey)) {
            // Fit on a side, give it a really good score.
            int Score = std::numeric_limits<int>::min() + (FreeBlocks[block].SizeX * FreeBlocks[block].SizeY - size);

            if (Score < BestFitScore) {
                BestFitScore = Score;
                BestFitIndex = block;
            }
        } else if ((FreeBlocks[block].SizeX > sizex) && (FreeBlocks[block].SizeY > sizey)) {
            // Still fits but not that good, give a neutral score.
            int Score = FreeBlocks[block].SizeX * FreeBlocks[block].SizeY - size;

            if (Score < BestFitScore) {
                BestFitScore = Score;
                BestFitIndex = block;
            }
        }
    }

    if (BestFitIndex == -1) {
        Engine::Error("Alloc2D::Alloc: Ran out of space!");
    }

    // Now, split the block!
    Block BestFitCopy = FreeBlocks[BestFitIndex]; // Copied because the block data gets changed during the process.

    // Matched an axis.
    if (BestFitScore < 0) {
        if (BestFitCopy.SizeX == sizex) {
            FreeBlocks[BestFitIndex].SizeY -= sizey;
            FreeBlocks[BestFitIndex].PosY += sizey;
        } else {
            FreeBlocks[BestFitIndex].SizeX -= sizex;
            FreeBlocks[BestFitIndex].PosX += sizex;
        }

        return Block(BestFitCopy.PosX, BestFitCopy.PosY, sizex, sizey);
    }

    // Split.
    FreeBlocks[BestFitIndex].PosX += sizex;
    FreeBlocks[BestFitIndex].SizeX += sizex;
    FreeBlocks[BestFitIndex].SizeY = sizey;

    FreeBlocks.emplace_back(BestFitCopy.PosX, BestFitCopy.PosY + sizey, BestFitCopy.SizeX, BestFitCopy.SizeY - sizey); 

    return Block(BestFitCopy.PosX, BestFitCopy.PosY, sizex, sizey);
}

void Geometry::Alloc2D::Free(Geometry::Alloc2D::Block block) {
    Engine::Warning("Alloc2D::Free is not really supported right now, sorry(lacks coalescence)");
    FreeBlocks.push_back(block);
}