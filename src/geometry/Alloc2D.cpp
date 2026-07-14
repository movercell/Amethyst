#include "engine/geometry/Alloc2D.h"

Geometry::Alloc2D::Block Geometry::Alloc2D::Alloc(uint16_t sizex, uint16_t sizey) {
    if (AlignmentX > 1) sizex = ((sizex + AlignmentX - 1) / AlignmentX) * AlignmentX; // Alignment.
    if (AlignmentY > 1) sizey = ((sizey + AlignmentY - 1) / AlignmentY) * AlignmentY;

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
    FreeBlocks[BestFitIndex].SizeX -= sizex;
    FreeBlocks[BestFitIndex].SizeY = sizey;

    FreeBlocks.emplace_back(BestFitCopy.PosX, BestFitCopy.PosY + sizey, BestFitCopy.SizeX, BestFitCopy.SizeY - sizey); 

    Block result = Block(BestFitCopy.PosX, BestFitCopy.PosY, sizex, sizey);

    if (AllocCallback) AllocCallback(result);

    return result;
}

#define REMOVE_CURRENT_BLOCK_AND_GO_BACK_TO_START   std::swap(FreeBlocks[i], FreeBlocks.back()); \
                                                    FreeBlocks.pop_back(); \
                                                    i = -1; \
                                                    continue;
void Geometry::Alloc2D::Free(Geometry::Alloc2D::Block block) { // Handles coalescence for any blocks that are next to the one being freed.
    if (AlignmentX > 1) block.SizeX = ((block.SizeX + AlignmentX - 1) / AlignmentX) * AlignmentX; // Alignment.
    if (AlignmentY > 1) block.SizeY = ((block.SizeY + AlignmentY - 1) / AlignmentY) * AlignmentY;

    for (int i = 0; i < FreeBlocks.size(); i++) {
        if (FreeBlocks[i].SizeX == block.SizeX) {
            if ((FreeBlocks[i].PosY + FreeBlocks[i].SizeY) == block.PosY) {
                block.PosY -= FreeBlocks[i].SizeY;
                block.SizeY += FreeBlocks[i].SizeY;

                REMOVE_CURRENT_BLOCK_AND_GO_BACK_TO_START
            }
            if ((block.PosY + block.SizeY) == FreeBlocks[i].PosY) {
                block.SizeY += FreeBlocks[i].SizeY;

                REMOVE_CURRENT_BLOCK_AND_GO_BACK_TO_START
            }
        }

        if (FreeBlocks[i].SizeY == block.SizeY) {
            if ((FreeBlocks[i].PosX + FreeBlocks[i].SizeX) == block.PosX) {
                block.PosX -= FreeBlocks[i].SizeX;
                block.SizeX += FreeBlocks[i].SizeX;

                REMOVE_CURRENT_BLOCK_AND_GO_BACK_TO_START
            }
            if ((block.PosX + block.SizeX) == FreeBlocks[i].PosX) {
                block.SizeX += FreeBlocks[i].SizeX;

                REMOVE_CURRENT_BLOCK_AND_GO_BACK_TO_START
            }
        }
    }
    FreeBlocks.push_back(block);
    
    if (FreeCallback) FreeCallback(block);
}