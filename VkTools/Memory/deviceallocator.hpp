#pragma once
#include "abstractallocator.hpp"
#include "chunkallocator.hpp"

class DeviceAllocator : public AbstractAllocator
{
public:
    DeviceAllocator(Device device, vk::DeviceSize size);

    Block allocate(vk::DeviceSize size, vk::DeviceSize alignment, int memoryTypeIndex);
    void deallocate(Block &block);


private:
    ChunkAllocator mChunkAllocator;
    std::vector<std::shared_ptr<Chunk>> mChunks;
};
