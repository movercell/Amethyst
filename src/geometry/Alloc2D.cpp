#include "engine/geometry/Alloc2D.h"

#define REMOVE_BLOCK(BLOCK_ID)  std::swap(FreeBlocks[BLOCK_ID], FreeBlocks.back()); \
                                FreeBlocks.pop_back();

Geometry::Alloc2D::Block Geometry::Alloc2D::RawAlloc(uint16_t sizex, uint16_t sizey) {
    int BestFitIndex = -1;
    int BestFitScore = std::numeric_limits<int>::max(); // The lower, the better.
    Block result;
    int size = sizex * sizey;

    // Scoring logic.
    for (int block = 0; block < FreeBlocks.size(); block++) {
        if ((FreeBlocks[block].SizeX == sizex) && (FreeBlocks[block].SizeY == sizey)) {
            // Perfect fit, nice!
            result = FreeBlocks[block];
            std::swap(FreeBlocks[block], FreeBlocks.back());
            FreeBlocks.pop_back();

            if (AllocCallback) AllocCallback(result);
            return result;
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
    Block BestFitCopy = FreeBlocks[BestFitIndex]; // Copied because the block gets removed during the process.
    REMOVE_BLOCK(BestFitIndex)

    // Matched an axis.
    if (BestFitScore < 0) {
        if (BestFitCopy.SizeX == sizex) {
            RawFree({BestFitCopy.PosX, (uint16_t)(BestFitCopy.PosY + sizey), BestFitCopy.SizeX, (uint16_t)(BestFitCopy.SizeY - sizey)});
        } else {
            RawFree({(uint16_t)(BestFitCopy.PosX + sizex), BestFitCopy.PosY, (uint16_t)(BestFitCopy.SizeX - sizex), BestFitCopy.SizeY});
        }

        result = Block(BestFitCopy.PosX, BestFitCopy.PosY, sizex, sizey);
        if (AllocCallback) AllocCallback(result);
        return result;
    }

    // Split.
    RawFree({(uint16_t)(BestFitCopy.PosX + sizex), BestFitCopy.PosY, (uint16_t)(BestFitCopy.SizeX - sizex), sizey}); 
    RawFree({BestFitCopy.PosX, (uint16_t)(BestFitCopy.PosY + sizey), BestFitCopy.SizeX, (uint16_t)(BestFitCopy.SizeY - sizey)}); 

    result = Block(BestFitCopy.PosX, BestFitCopy.PosY, sizex, sizey);
    if (AllocCallback) AllocCallback(result);
    return result;
}
Geometry::Alloc2D::Block Geometry::Alloc2D::Alloc(uint16_t sizex, uint16_t sizey) {
    uint16_t passedsizex = sizex;
    uint16_t passedsizey = sizey;
    sizex += PadX * 2;
    sizey += PadY * 2;

    if (AlignmentX > 1) sizex = ((sizex + AlignmentX - 1) / AlignmentX) * AlignmentX; // Alignment.
    if (AlignmentY > 1) sizey = ((sizey + AlignmentY - 1) / AlignmentY) * AlignmentY;

    Block Allocated = RawAlloc(sizex, sizey);
    Allocated.PosX += PadX;
    Allocated.PosY += PadY;
    Allocated.SizeX = passedsizex;
    Allocated.SizeY = passedsizey;

    return Allocated;
}

#define REMOVE_CURRENT_BLOCK_AND_GO_BACK_TO_START   REMOVE_BLOCK(i) \
                                                    i = -1; \
                                                    continue;
void Geometry::Alloc2D::RawFree(Geometry::Alloc2D::Block block) { // Handles coalescence for any blocks that are next to the one being freed.
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
void Geometry::Alloc2D::Free(Geometry::Alloc2D::Block block) {
    block.SizeX += PadX * 2;
    block.SizeY += PadY * 2;
    block.PosX -= PadX;
    block.PosY -= PadY;
    if (AlignmentX > 1) block.SizeX = ((block.SizeX + AlignmentX - 1) / AlignmentX) * AlignmentX; // Alignment.
    if (AlignmentY > 1) block.SizeY = ((block.SizeY + AlignmentY - 1) / AlignmentY) * AlignmentY;

    RawFree(block);
}