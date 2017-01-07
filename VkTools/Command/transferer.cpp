#include "transferer.hpp"

Transferer::Transferer(uint32_t numberBuffers, vk::DeviceSize sizeTransfererBuffers,
                       std::shared_ptr<AbstractAllocator> allocator, CommandBufferSubmitter &commandBufferSubmitter) :
    mAllocator(allocator),
    mCommandBufferSubmitter(std::make_shared<CommandBufferSubmitter>(commandBufferSubmitter)),
    mSizeTransfererBuffers(std::make_shared<uint32_t>(sizeTransfererBuffers)) {
    for(uint32_t i = 0; i < numberBuffers; ++i)
        addTransferBuffer();
}

std::shared_ptr<AbstractAllocator> Transferer::getAllocator() {
    return mAllocator;
}

void Transferer::addTransferBuffer() {
    mTransfererBuffers->emplace_back(Buffer(mAllocator->getDevice(), vk::BufferUsageFlagBits::eTransferSrc, *mSizeTransfererBuffers, mAllocator, false));
    mSizeAlreadyUsed->emplace_back(0);
}

void Transferer::cacheResource(std::shared_ptr<VkResource> resource) {
    mCommandBufferSubmitter->cacheResource(resource);
}

void Transferer::notify() {
    *mIndex = 0;
    for(auto &size : *mSizeAlreadyUsed)
        size = 0;
}

void Transferer::transfer(Buffer const &src, Buffer &dst, vk::BufferCopy bufferCopy) {
    // Check if size and usage are legals
    assert((src.getUsage() & vk::BufferUsageFlagBits::eTransferSrc) ==
                vk::BufferUsageFlagBits::eTransferSrc);
    assert((dst.getUsage() & vk::BufferUsageFlagBits::eTransferDst) ==
                vk::BufferUsageFlagBits::eTransferDst);

    auto newSize = bufferCopy.dstOffset + bufferCopy.size;
    assert(src.getSize() >= bufferCopy.srcOffset + bufferCopy.size);

    if(dst.getSize() < newSize) {
        Buffer newBuffer(dst.getDevice(), dst.getUsage(), std::max(dst.getSize() * 2, newSize), dst.getAllocator(), dst.isDeviceLocal());

        if(bufferCopy.dstOffset > 0)
            transfer(dst, newBuffer, vk::BufferCopy(0, 0, bufferCopy.dstOffset));

        // Ensure dst buffer is not destroyed before prior transfer completed
        cacheResource(std::make_shared<Buffer>(dst));
        dst = newBuffer;
    }

    vk::CommandBufferBeginInfo begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    vk::CommandBuffer cmd = mCommandBufferSubmitter->createCommandBuffer(this);

    cmd.begin(begin);
    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
                        vk::PipelineStageFlagBits::eTransfer,
                        vk::DependencyFlags(),
                        nullptr,
                        vk::BufferMemoryBarrier(vk::AccessFlagBits::eMemoryWrite,
                                                vk::AccessFlagBits::eTransferRead,
                                                VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
                                                src, bufferCopy.srcOffset, bufferCopy.size),
                        nullptr);
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

void Transferer::transfer(Buffer &buffer, vk::DeviceSize offset, vk::DeviceSize size, void const *data) {
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

vk::ImageMemoryBarrier Transferer::transitionImage(Image image,
                                                   vk::ImageLayout oldLayout,
                                                   vk::ImageLayout newLayout,
                                                   vk::ImageSubresourceRange imageSubResourceRange) {
    vk::AccessFlags src, dst;

    if(oldLayout == vk::ImageLayout::eUndefined)
        src = vk::AccessFlags();

    // If it was preinitialized: the host write on it and the memory barrier is implicit
    else if(oldLayout == vk::ImageLayout::ePreinitialized)
        src = vk::AccessFlagBits::eHostWrite; // Must be useless since the barrier is already performed : it is only for layer

    // If it was in transferDst, we wait for transferWrite
    else if(oldLayout == vk::ImageLayout::eTransferDstOptimal)
        src = vk::AccessFlagBits::eTransferWrite;

    // If it was in transferSrc, we wait for transferRead
    else if(oldLayout == vk::ImageLayout::eTransferSrcOptimal)
        src = vk::AccessFlagBits::eTransferRead; // Useless since make available a read is meaningless

    else
        assert(!"This oldLayout is not managed");

    // If it will be transferSrcOptimal, it should be ready at transferRead
    if(newLayout == vk::ImageLayout::eTransferSrcOptimal)
        dst = vk::AccessFlagBits::eTransferRead;

    // If it will be transferDstOptimal
    else if(newLayout == vk::ImageLayout::eTransferDstOptimal)
        dst = vk::AccessFlagBits::eTransferWrite; // Myst be useless since invalidate for a write

    // If it will be shaderReadOnlyOptimal, it should be ready at shaderRead
    else if(newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        dst = vk::AccessFlagBits::eShaderRead;

    else
        assert(!"This newLayout is not managed");

    return vk::ImageMemoryBarrier(src, dst, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
                                  image, imageSubResourceRange);
}

void Transferer::transfer(const Image &src, Image &dst, vk::ImageLayout oldSrcLayout, vk::ImageLayout oldDstLayout, vk::ImageLayout newSrcLayout, vk::ImageLayout newDstLayout, vk::ImageSubresourceRange srcImageSubResourceRange, vk::ImageSubresourceRange dstImageSubResourceRange, vk::ImageSubresourceLayers srcSubResource, vk::ImageSubresourceLayers dstSubResource, vk::Offset3D srcOffset, vk::Offset3D dstOffset, vk::Extent3D extent) {
    assert((src.getUsage() & vk::ImageUsageFlagBits::eTransferSrc) ==
                vk::ImageUsageFlagBits::eTransferSrc);

    assert((dst.getUsage() & vk::ImageUsageFlagBits::eTransferDst) ==
                vk::ImageUsageFlagBits::eTransferDst);

    vk::CommandBuffer cmd = mCommandBufferSubmitter->createCommandBuffer(this);

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmd.begin(beginInfo);

    std::vector<vk::ImageMemoryBarrier> preBarrier =
        {transitionImage(src, oldSrcLayout, vk::ImageLayout::eTransferSrcOptimal, srcImageSubResourceRange),
         transitionImage(dst, oldDstLayout, vk::ImageLayout::eTransferDstOptimal, dstImageSubResourceRange)};

    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, // host stage is implicitly done by buffersubmission
                        vk::PipelineStageFlagBits::eTransfer,
                        vk::DependencyFlags(),
                        nullptr, nullptr,
                        preBarrier);

    cmd.copyImage(src, vk::ImageLayout::eTransferSrcOptimal,
                  dst, vk::ImageLayout::eTransferDstOptimal,
                  vk::ImageCopy(srcSubResource, srcOffset,
                                dstSubResource, dstOffset,
                                extent));

    std::vector<vk::ImageMemoryBarrier> postBarrier =
        {transitionImage(src, vk::ImageLayout::eTransferSrcOptimal, newSrcLayout, srcImageSubResourceRange),
         transitionImage(dst, vk::ImageLayout::eTransferDstOptimal, newDstLayout, dstImageSubResourceRange)};

    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                        vk::PipelineStageFlagBits::eAllCommands,
                        vk::DependencyFlags(),
                        nullptr, nullptr,
                        postBarrier);

    cmd.end();
}

void Transferer::buildMipMap(Image &src) {
    vk::CommandBuffer cmd = mCommandBufferSubmitter->createCommandBuffer(this);
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmd.begin(beginInfo);

    // For each mipmap level, we blit the prior one to it
    for(uint32_t i = 1; i < src.getMipLevels(); ++i) {
        vk::ImageBlit blit;
        blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.srcSubresource.mipLevel = i - 1;
        blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        blit.dstSubresource.mipLevel = i;

        // each mipmap is the size divided by two
        blit.srcOffsets[1] = vk::Offset3D(src.getSize().width >> (i - 1),
                                          src.getSize().height >> (i - 1),
                                          1);
        blit.dstOffsets[1] = vk::Offset3D(src.getSize().width >> i,
                                          src.getSize().height >> i,
                                          1);

        vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, i, 1, 0, 1);

        // transferDst go to transferSrc because this mipmap will be the source for the next iteration (the next level)
        vk::ImageMemoryBarrier preBlit = transitionImage(src, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, range);
        vk::ImageMemoryBarrier postBlit = transitionImage(src, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, range);

        // perform only the transition : does not need to block
        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                            vk::PipelineStageFlagBits::eTransfer,
                            vk::DependencyFlags(),
                            nullptr, nullptr,
                            preBlit);

        cmd.blitImage(src, vk::ImageLayout::eTransferSrcOptimal,
                      src, vk::ImageLayout::eTransferDstOptimal, blit,
                      vk::Filter::eLinear);

        // This barrier ensure the transfer is finished and transition the image to srcOptimal
        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                            vk::PipelineStageFlagBits::eTransfer,
                            vk::DependencyFlags(),
                            nullptr, nullptr,
                            postBlit);
    }

    vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, VK_REMAINING_MIP_LEVELS, 0, 1);

    // transition all mipmap levels to shaderReadOnlyOptimal
    vk::ImageMemoryBarrier transition = transitionImage(src,
                                                        vk::ImageLayout::eTransferSrcOptimal,
                                                        vk::ImageLayout::eShaderReadOnlyOptimal,
                                                        range);

    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                        vk::PipelineStageFlagBits::eAllCommands,
                        vk::DependencyFlags(),
                        nullptr, nullptr, transition);

    cmd.end();
}
