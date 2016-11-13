#pragma once
#include "../System/commandbuffersubmitter.hpp"
#include "buffer.hpp"

class BufferTransferer : public ObserverCommandBufferSubmitter
{
public:
    BufferTransferer(Device &device, uint32_t numberBuffers, vk::DeviceSize sizeTransfererBuffers,
                     std::shared_ptr<AbstractAllocator> allocator, CommandBufferSubmitter &commandBufferSubmitter);

    void transfer(const Buffer &src, Buffer &dst,
                  vk::DeviceSize offsetSrc,
                  vk::DeviceSize offsetDst,
                  vk::DeviceSize size);

    void transfer(Buffer &buffer, vk::DeviceSize offset, vk::DeviceSize size, void *data);

    void notify();

private:
    std::shared_ptr<CommandBufferSubmitter> mCommandBufferSubmitter;
    std::shared_ptr<std::vector<Buffer>> mTransfererBuffers = std::make_shared<std::vector<Buffer>>();
    std::shared_ptr<uint32_t> mSizeTransfererBuffers;
    std::shared_ptr<uint32_t> mIndex = std::make_shared<uint32_t>(0);
};
