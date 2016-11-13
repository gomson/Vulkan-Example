#include "buffertransferer.hpp"

BufferTransferer::BufferTransferer(Device &device, uint32_t numberBuffers, vk::DeviceSize sizeTransfererBuffers,
                                   std::shared_ptr<AbstractAllocator> allocator, CommandBufferSubmitter &commandBufferSubmitter) :
    mCommandBufferSubmitter(std::make_shared<CommandBufferSubmitter>(commandBufferSubmitter)),
    mSizeTransfererBuffers(std::make_shared<uint32_t>(sizeTransfererBuffers)) {
    mCommandBufferSubmitter->addObserver(this);
    mTransfererBuffers->resize(numberBuffers);
    for(auto &buffer : *mTransfererBuffers)
        buffer = Buffer(device, vk::BufferUsageFlagBits::eTransferSrc, sizeTransfererBuffers, allocator, false);
}

void BufferTransferer::notify() {
    *mIndex = 0;
}

void BufferTransferer::transfer(Buffer const &src, Buffer &dst,
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

    vk::CommandBufferBeginInfo begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    vk::CommandBuffer cmd = mCommandBufferSubmitter->createCommandBuffer();

    cmd.begin(begin);
    cmd.copyBuffer(src, dst, {region});
    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                        vk::PipelineStageFlagBits::eAllCommands,
                        vk::DependencyFlags(),
                        nullptr,
                        vk::BufferMemoryBarrier(vk::AccessFlagBits::eTransferWrite,
                                                vk::AccessFlagBits::eMemoryRead,
                                                VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
                                                dst, offsetSrc, size),
                        nullptr);
    cmd.end();

}

void BufferTransferer::transfer(Buffer &buffer, vk::DeviceSize offset, vk::DeviceSize size, void *data) {
    assert(size <= *mSizeTransfererBuffers);
    
    if(*mIndex == mTransfererBuffers->size()) {
        mCommandBufferSubmitter->submit();
        mCommandBufferSubmitter->wait();
    }
    
    memcpy((*mTransfererBuffers)[*mIndex].getPtr(), data, size);
    transfer((*mTransfererBuffers)[*mIndex], buffer, 0, offset, size);
    (*mIndex)++;
}
