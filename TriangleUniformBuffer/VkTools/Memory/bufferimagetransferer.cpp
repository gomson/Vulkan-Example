#include "bufferimagetransferer.hpp"
#include <iostream>

/*
void swap(BufferImageTransferer &b1, BufferImageTransferer &b2) {
    std::swap;
    swap(b1.mCommandBuffer, b2.mCommandBuffer);
    swap(b1.mCommandPool, b2.mCommandPool);
    swap(b1.mDevice, b2.mDevice);
    swap(b1.mFences, b2.mFences);
    swap(b1.mIndex, b2.mIndex);
    swap(b1.mQueue, b2.mQueue);
}

BufferImageTransferer::BufferImageTransferer(BufferImageTransferer &&bufferImageTransferer) {
    swap(*this, bufferImageTransferer);
}

BufferImageTransferer::BufferImageTransferer(const BufferImageTransferer &bufferImageTransferer) {
    mCommandBuffer = bufferImageTransferer.mCommandBuffer;
    mCommandPool = bufferImageTransferer.mCommandPool;
    mDevice =
}
*/
BufferImageTransferer::BufferImageTransferer(Device &device, uint32_t numberCommandBuffer) :
    mDevice(std::make_shared<Device>(device)),
    mCommandPool(std::make_shared<CommandPool>(device, false, true, device.getIndexTransferQueue())),
    mQueue(std::make_shared<vk::Queue>(device.getTransferQueue())) {
    *mCommandBuffer = mCommandPool->allocate(vk::CommandBufferLevel::ePrimary, numberCommandBuffer);
    for(uint32_t i = 0; i < numberCommandBuffer; ++i)
        mFences->emplace_back(device, true);
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

    (*mFences)[*mIndex].wait();
    (*mFences)[*mIndex].reset();

    vk::CommandBufferBeginInfo begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    vk::CommandBuffer cmd = (*mCommandBuffer)[*mIndex];

    cmd.begin(begin);
    cmd.copyBuffer(src, dst, {region});
    cmd.end();

    vk::SubmitInfo submit(0, nullptr, nullptr, 1, &cmd, 0, nullptr);

    mQueue->submit({submit}, (*mFences)[*mIndex]);

    *mIndex = (*mIndex + 1) % (*mFences).size();
}

void BufferImageTransferer::flush() {
    mIndex = 0;
    mDevice->waitForFences(std::vector<vk::Fence>(mFences->begin(), mFences->end()), true, UINT64_MAX);
}
