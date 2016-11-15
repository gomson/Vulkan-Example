#include "buffertransferer.hpp"

BufferTransferer::BufferTransferer(const Device &device, uint32_t numberBuffers, vk::DeviceSize sizeTransfererBuffers,
                                   std::shared_ptr<AbstractAllocator> allocator, CommandBufferSubmitter &commandBufferSubmitter) :
    mCommandBufferSubmitter(std::make_shared<CommandBufferSubmitter>(commandBufferSubmitter)),
    mSizeTransfererBuffers(std::make_shared<uint32_t>(sizeTransfererBuffers)) {
    mCommandBufferSubmitter->addObserver(this);
    mTransfererBuffers->resize(numberBuffers);
    mSizeAlreadyUsed->resize(numberBuffers, 0);

    for(auto &buffer : *mTransfererBuffers)
        buffer = Buffer(device, vk::BufferUsageFlagBits::eTransferSrc, sizeTransfererBuffers, allocator, false);
}

void BufferTransferer::notify() {
    *mIndex = 0;
    for(auto &size : *mSizeAlreadyUsed)
        size = 0;
}

void BufferTransferer::transfer(Buffer const &src, Buffer &dst, vk::BufferCopy bufferCopy) {
    // Check if size and usage are legals
    assert((src.getUsage() & vk::BufferUsageFlagBits::eTransferSrc) ==
                vk::BufferUsageFlagBits::eTransferSrc);
    assert((dst.getUsage() & vk::BufferUsageFlagBits::eTransferDst) ==
                vk::BufferUsageFlagBits::eTransferDst);

    auto newSize = bufferCopy.dstOffset + bufferCopy.size;
    assert(src.getSize() >= bufferCopy.srcOffset + bufferCopy.size);

    Buffer newBuffer; // usefull if realloc;
    bool realloc = false;
    if(dst.getSize() < (newSize)) {
        newBuffer = Buffer(dst.getDevice(), dst.getUsage(), std::max(dst.getSize() * 2, newSize), dst.getAllocator(), dst.isDeviceLocal());

        if(bufferCopy.dstOffset > 0)
            transfer(dst, newBuffer, vk::BufferCopy(0, 0, bufferCopy.dstOffset));

        std::swap(dst, newBuffer);
        realloc = true;
    }

    vk::CommandBufferBeginInfo begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    vk::CommandBuffer cmd = mCommandBufferSubmitter->createCommandBuffer();

    cmd.begin(begin);
    cmd.copyBuffer(src, dst, bufferCopy);
    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                        vk::PipelineStageFlagBits::eAllCommands,
                        vk::DependencyFlags(),
                        nullptr,
                        vk::BufferMemoryBarrier(vk::AccessFlagBits::eTransferWrite,
                                                vk::AccessFlagBits::eMemoryRead,
                                                VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
                                                dst, bufferCopy.dstOffset, bufferCopy.size),
                        nullptr);
    cmd.end();

    // After it, newBuffer will be destroyed
    if(realloc)
        mCommandBufferSubmitter->submit(true);
}

void BufferTransferer::transfer(Buffer &buffer, vk::DeviceSize offset, vk::DeviceSize size, void *data) {
    if(*mIndex == mTransfererBuffers->size())
        mCommandBufferSubmitter->submit(true);

    auto availableSize = *mSizeTransfererBuffers - (*mSizeAlreadyUsed)[*mIndex];
    while(size > availableSize) {
        transfer(buffer, offset, availableSize, data);
        offset += availableSize;
        size -= availableSize;
        data = (char*)data + availableSize;
        (*mIndex)++;
        if(*mIndex == mTransfererBuffers->size())
            mCommandBufferSubmitter->submit(true);
        availableSize = *mSizeTransfererBuffers - (*mSizeAlreadyUsed)[*mIndex];
    }
    
    memcpy((*mTransfererBuffers)[*mIndex].getPtr(), data, size);
    transfer((*mTransfererBuffers)[*mIndex], buffer, vk::BufferCopy(0, offset, size));
    (*mSizeAlreadyUsed)[*mIndex] += size;
}
