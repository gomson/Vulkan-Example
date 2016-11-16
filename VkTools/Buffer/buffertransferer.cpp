#include "buffertransferer.hpp"

BufferTransferer::BufferTransferer(const Device &device, uint32_t numberBuffers, vk::DeviceSize sizeTransfererBuffers,
                                   std::shared_ptr<AbstractAllocator> allocator, CommandBufferSubmitter &commandBufferSubmitter) :
    mAllocator(allocator),
    mCommandBufferSubmitter(std::make_shared<CommandBufferSubmitter>(commandBufferSubmitter)),
    mSizeTransfererBuffers(std::make_shared<uint32_t>(sizeTransfererBuffers)) {
    for(uint32_t i = 0; i < numberBuffers; ++i)
        addTransferBuffer();
}

void BufferTransferer::addTransferBuffer() {
    mTransfererBuffers->emplace_back(Buffer(mAllocator->getDevice(), vk::BufferUsageFlagBits::eTransferSrc, *mSizeTransfererBuffers, mAllocator, false));
    mSizeAlreadyUsed->emplace_back(0);
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

    if(dst.getSize() < (newSize)) {
        Buffer newBuffer(dst.getDevice(), dst.getUsage(), std::max(dst.getSize() * 2, newSize), dst.getAllocator(), dst.isDeviceLocal());

        if(bufferCopy.dstOffset > 0)
            transfer(dst, newBuffer, vk::BufferCopy(0, 0, bufferCopy.dstOffset));

        // Ensure dst buffer is not destroyed before prior transfer completed
        mCommandBufferSubmitter->cacheBuffer(dst);
        dst = newBuffer;
    }

    vk::CommandBufferBeginInfo begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    vk::CommandBuffer cmd = mCommandBufferSubmitter->createCommandBuffer(this);

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
}

void BufferTransferer::transfer(Buffer &buffer, vk::DeviceSize offset, vk::DeviceSize size, void *data) {
    if(*mIndex == mTransfererBuffers->size())
        addTransferBuffer();

    // size available in the current transferBuffer
    decltype(size) availableSize;
    // We reduce the size to transfer by the availableSize in the current transferBuffer
    while(size > (availableSize = *mSizeTransfererBuffers - (*mSizeAlreadyUsed)[*mIndex])) {
        // Only one level of recursion
        transfer(buffer, offset, availableSize, data); // transfer the buffer
        // Update new values
        offset += availableSize;
        size -= availableSize;
        data = (char*)data + availableSize;

        // go to the next transferBuffer
        (*mIndex)++;
        if(*mIndex == mTransfererBuffers->size())
            addTransferBuffer();
    }
    
    // CPU to HOST_VISIBLE
    memcpy((char*)(*mTransfererBuffers)[*mIndex].getPtr() + (*mSizeAlreadyUsed)[*mIndex], data, size);
    transfer((*mTransfererBuffers)[*mIndex], buffer, vk::BufferCopy((*mSizeAlreadyUsed)[*mIndex], offset, size));
    (*mSizeAlreadyUsed)[*mIndex] += size;
}
