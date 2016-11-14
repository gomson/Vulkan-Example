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

    if((device.getPhysicalDevice().getMemoryProperties().memoryTypes[memoryTypeIndex].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible) {
        mPtr = block.ptr = device.mapMemory(mMemory, 0, VK_WHOLE_SIZE);
    }

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

bool Chunk::allocate(vk::DeviceSize size, vk::DeviceSize alignment, Block &block) {
    // if chunk is too small
    if(size > mSize)
        return false;

    for(uint32_t i = 0; i < mBlocks.size(); ++i) {
        if(mBlocks[i].free) {
            // Compute size after taking care about offsetAlignment
            uint32_t newSize = mBlocks[i].size;

            if(mBlocks[i].offset % alignment != 0)
                newSize -= alignment - mBlocks[i].offset % alignment;

            if(newSize >= size) {
                // if perfect match
                mBlocks[i].size = newSize;
                if(mBlocks[i].offset % alignment != 0)
                    mBlocks[i].offset += alignment - mBlocks[i].offset % alignment;
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
                nextBlock.ptr = (char*)mBlocks[i].ptr + size;
                mBlocks.emplace_back(nextBlock);

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
