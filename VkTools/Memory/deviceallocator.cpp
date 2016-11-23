#include "deviceallocator.hpp"

DeviceAllocator::DeviceAllocator(const Device &device, vk::DeviceSize size) :
    AbstractAllocator(device),
    mChunkAllocator(device, size) {

}

Block DeviceAllocator::allocate(vk::DeviceSize size, vk::DeviceSize alignment, int memoryTypeIndex) {
    Block block;
    // We search a "good" chunk
    for(auto &chunk : mChunks)
        if(chunk->memoryTypeIndex() == memoryTypeIndex)
            if(chunk->allocate(size, alignment, block))
                return block;

    mChunks.emplace_back(mChunkAllocator.allocate(size, memoryTypeIndex));
    assert(mChunks.back()->allocate(size, alignment, block));
    return block;
}

void DeviceAllocator::deallocate(Block &block) {
    for(auto &chunk : mChunks) {
        if(chunk->isIn(block)) {
            chunk->deallocate(block);
            return ;
        }
    }
    assert(!"unable to deallocate the block");
}
