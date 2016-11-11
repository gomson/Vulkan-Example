#include "chunk.hpp"

Chunk::Chunk(Device &device, vk::DeviceSize size, int memoryTypeIndex) :
    mDevice(device),
    mSize(size),
    mMemoryTypeIndex(memoryTypeIndex) {
    vk::MemoryAllocateInfo allocateInfo(size, memoryTypeIndex);

    Block block;
    block.free = true;
    block.offset = 0;
    block.size = size;
    mMemory = block.memory = device.allocateMemory(allocateInfo);

    mBlocks.emplace_back(block);
}

int Chunk::memoryTypeIndex() const {
    return mMemoryTypeIndex;
}

bool Chunk::isIn(const Block &block) const {
    return block.memory == mMemory;
}

void Chunk::deallocate(const Block &block) {
    auto blockIt(std::find(mBlocks.begin(), mBlocks.end(), block));
    assert(blockIt != mBlocks.end());
    // Just put the block to free
    blockIt->free = true;
}

bool Chunk::allocate(vk::DeviceSize size, Block &block) {
    // for alignment
    if(size % 256 != 0)
        size = size + (256 - size % 256);

    // if chunk is too small
    if(size > mSize)
        return false;

    for(uint32_t i = 0; i < mBlocks.size(); ++i) {
        if(mBlocks[i].free) {
            // if match
            if(mBlocks[i].size >= size) {
                // if perfect match
                if(mBlocks[i].size == size) {
                    mBlocks[i].free = false;
                    block = mBlocks[i];
                    return true;
                }

                Block nextBlock;
                nextBlock.free = true;
                nextBlock.offset = mBlocks[i].offset + size;
                nextBlock.memory = mMemory;
                nextBlock.size = mBlocks[i].size - size;

                mBlocks[i].size = size;
                mBlocks[i].free = false;

                block = mBlocks[i];
                return true;
            }
        }
    }

    return false;
}

Chunk::~Chunk() {
    mDevice.freeMemory(mMemory);
}
