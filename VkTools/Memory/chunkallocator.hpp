#pragma once
#include "chunk.hpp"

class ChunkAllocator : private NotCopyable
{
public:
    ChunkAllocator(Device &device, vk::DeviceSize size);

    // if size > mSize, allocate to the next power of 2
    std::unique_ptr<Chunk> allocate(vk::DeviceSize size, int memoryTypeIndex);

private:
    Device mDevice;
    vk::DeviceSize mSize;
};
