#include "deviceallocator.hpp"

DeviceAllocator::DeviceAllocator(Device device, vk::DeviceSize size) :
    mDevice(device),
    mChunkAllocator(device, size) {

}

Block DeviceAllocator::allocate(vk::DeviceSize size, int memoryTypeIndex) {
    Block block;
    // We search a "good" chunk
    for(auto &chunk : mChunks)
        if(chunk->memoryTypeIndex() == memoryTypeIndex)
            if(chunk->allocate(size, block))
                return block;

    mChunks.emplace_back(mChunkAllocator.allocate(size, memoryTypeIndex));
    assert(mChunks.back()->allocate(size, block));
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
