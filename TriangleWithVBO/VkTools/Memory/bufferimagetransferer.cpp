#include "bufferimagetransferer.hpp"

BufferImageTransferer::BufferImageTransferer(Device &device, uint32_t numberCommandBuffer) :
    mDevice(device),
    mCommandPool(device, false, true, device.getIndexTransferQueue()),
    mFences(numberCommandBuffer),
    mQueue(device.getTransferQueue())
{
    mCommandBuffer = mCommandPool.allocate(vk::CommandBufferLevel::ePrimary, numberCommandBuffer);
    for(auto &fence : mFences)
        fence = std::make_unique<Fence>(device, true);
}

void BufferImageTransferer::transfer(Buffer const &src, Buffer &dst,
                                     vk::DeviceSize offsetSrc, vk::DeviceSize offsetDst,
                                     vk::DeviceSize size) {
    // Check if size and usage are legals
    assert((src.getUsage() & vk::BufferUsageFlagBits::eTransferSrc) ==
                vk::BufferUsageFlagBits::eTransferSrc);
    assert((dst.getUsage() & vk::BufferUsageFlagBits::eTransferDst) ==
                vk::BufferUsageFlagBits::eTransferDst);

    assert(src.getSize() >= (offsetSrc + size));
    assert(dst.getSize() >= (offsetDst + size));

    // Prepare the region copied
    vk::BufferCopy region(offsetSrc, offsetDst, size);

    mFences[mIndex]->wait();
    mFences[mIndex]->reset();

    vk::CommandBufferBeginInfo begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    vk::CommandBuffer cmd = mCommandBuffer[mIndex];

    cmd.begin(begin);
    cmd.copyBuffer(src, dst, {region});
    cmd.end();

    vk::SubmitInfo submit(0, nullptr, nullptr, 1, &cmd, 0, nullptr);

    mQueue.submit({submit}, *mFences[mIndex]);

    mIndex = (mIndex + 1) % mFences.size();
}

void BufferImageTransferer::flush() {
    mIndex = 0;
    for(auto &fence : mFences)
        fence->wait();
}
