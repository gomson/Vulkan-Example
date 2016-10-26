#pragma once
#include "block.hpp"

class Chunk : private NotCopyable {
public:
    Chunk(Device &device, vk::DeviceSize size, int memoryTypeIndex);

    bool allocate(vk::DeviceSize size, Block &block);
    bool isIn(Block const &block) const;
    void deallocate(Block const &block);
    int memoryTypeIndex() const;

    ~Chunk();

protected:
    Device mDevice;
    vk::DeviceMemory mMemory = VK_NULL_HANDLE;
    vk::DeviceSize mSize;
    int mMemoryTypeIndex;
    std::vector<Block> mBlocks;
};
