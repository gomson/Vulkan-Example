#pragma once
#include "chunk.hpp"

class ChunkAllocator
{
public:
    ChunkAllocator(vk::Device &device, vk::DeviceSize size);

    // if size > mSize, allocate to the next power of 2
    std::unique_ptr<Chunk> allocate(vk::DeviceSize size, int memoryTypeIndex);

private:
    vk::Device mDevice;
    vk::DeviceSize mSize;
};
