#pragma once
#include "buffer.hpp"
#include "VkTools/System/commandpool.hpp"
#include "VkTools/System/fence.hpp"

class BufferImageTransferer
{
public:
    BufferImageTransferer(Device &device, uint32_t numberCommandBuffer);

    BufferImageTransferer(BufferImageTransferer &&bufferImageTransferer) = default;
    BufferImageTransferer(BufferImageTransferer const &bufferImageTransferer) = default;
    BufferImageTransferer &operator=(BufferImageTransferer const &bufferImageTransferer) = default;

    void transfer(const Buffer &src, Buffer &dst,
                  vk::DeviceSize offsetSrc,
                  vk::DeviceSize offsetDst,
                  vk::DeviceSize size);

    void flush();

private:
    std::shared_ptr<Device> mDevice = std::make_shared<Device>();
    std::shared_ptr<CommandPool> mCommandPool;
    std::shared_ptr<std::vector<vk::CommandBuffer>> mCommandBuffer = std::make_shared<std::vector<vk::CommandBuffer>>();
    std::shared_ptr<std::vector<Fence>> mFences = std::make_shared<std::vector<Fence>>();
    std::shared_ptr<vk::Queue> mQueue = std::make_shared<vk::Queue>();
    std::shared_ptr<uint32_t> mIndex = std::make_shared<uint32_t>(0);
};
