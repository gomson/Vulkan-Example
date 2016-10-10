#pragma once
#include "buffer.hpp"
#include "VkTools/System/commandpool.hpp"
#include "VkTools/System/fence.hpp"

class BufferImageTransferer : private NotCopyable
{
public:
    BufferImageTransferer(Device &device, uint32_t numberCommandBuffer);

    void transfer(const Buffer &src, Buffer &dst,
                  vk::DeviceSize offsetSrc,
                  vk::DeviceSize offsetDst,
                  vk::DeviceSize size);

    void flush();

private:
    vk::Device mDevice;
    CommandPool mCommandPool;
    std::vector<vk::CommandBuffer> mCommandBuffer;
    std::vector<Fence> mFences;
    vk::Queue mQueue;
    uint32_t mIndex = 0;
};
