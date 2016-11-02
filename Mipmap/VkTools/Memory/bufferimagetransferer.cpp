#include "bufferimagetransferer.hpp"
#include <iostream>

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

void BufferImageTransferer::transfer(const Image &src, Image &dst,
                                     vk::ImageLayout oldSrcLayout, vk::ImageLayout oldDstLayout,
                                     vk::ImageLayout newSrcLayout, vk::ImageLayout newDstLayout,
                                     vk::ImageSubresourceRange srcImageSubResourceRange,
                                     vk::ImageSubresourceRange dstImageSubResourceRange,
                                     vk::ImageSubresourceLayers srcSubResource,
                                     vk::ImageSubresourceLayers dstSubResource,
                                     vk::Offset3D srcOffset,
                                     vk::Offset3D dstOffset,
                                     vk::Extent3D extent) {
    assert((src.getUsage() & vk::ImageUsageFlagBits::eTransferSrc) ==
                vk::ImageUsageFlagBits::eTransferSrc);

    assert((dst.getUsage() & vk::ImageUsageFlagBits::eTransferDst) ==
                vk::ImageUsageFlagBits::eTransferDst);

    (*mFences)[*mIndex].wait();
    (*mFences)[*mIndex].reset();

    vk::CommandBuffer cmd = (*mCommandBuffer)[*mIndex];

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmd.begin(beginInfo);

    std::vector<vk::ImageMemoryBarrier> preBarrier =
        {transitionImage(src, oldSrcLayout, vk::ImageLayout::eTransferSrcOptimal, srcImageSubResourceRange),
         transitionImage(dst, oldDstLayout, vk::ImageLayout::eTransferDstOptimal, dstImageSubResourceRange)};

    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                        vk::PipelineStageFlagBits::eTopOfPipe,
                        vk::DependencyFlagBits::eByRegion,
                        vk::ArrayProxy<const vk::MemoryBarrier>(nullptr),
                        vk::ArrayProxy<const vk::BufferMemoryBarrier>(nullptr),
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
                        vk::PipelineStageFlagBits::eTopOfPipe,
                        vk::DependencyFlagBits::eByRegion,
                        vk::ArrayProxy<const vk::MemoryBarrier>(nullptr),
                        vk::ArrayProxy<const vk::BufferMemoryBarrier>(nullptr),
                        postBarrier);

    cmd.end();

    vk::SubmitInfo submit(0, nullptr, nullptr, 1, &cmd, 0, nullptr);
    mQueue->submit({submit}, (*mFences)[*mIndex]);
    *mIndex = (*mIndex + 1) % (*mFences).size();
}

void BufferImageTransferer::buildMipMap(Image &src) {
    (*mFences)[*mIndex].wait();
    (*mFences)[*mIndex].reset();
    vk::CommandBuffer cmd = (*mCommandBuffer)[*mIndex];
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmd.begin(beginInfo);

    // For each mipmap level, we blit the prior one to it
    for(int i = 1; i < src.getMipLevels(); ++i) {
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
        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                            vk::PipelineStageFlagBits::eTopOfPipe,
                            vk::DependencyFlagBits::eByRegion,
                            vk::ArrayProxy<const vk::MemoryBarrier>(nullptr),
                            vk::ArrayProxy<const vk::BufferMemoryBarrier>(nullptr),
                            preBlit);

        cmd.blitImage(src, vk::ImageLayout::eTransferSrcOptimal,
                      src, vk::ImageLayout::eTransferDstOptimal, blit,
                      vk::Filter::eLinear);

        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                            vk::PipelineStageFlagBits::eTopOfPipe,
                            vk::DependencyFlagBits::eByRegion,
                            vk::ArrayProxy<const vk::MemoryBarrier>(nullptr),
                            vk::ArrayProxy<const vk::BufferMemoryBarrier>(nullptr),
                            postBlit);
    }

    vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, VK_REMAINING_MIP_LEVELS, 0, 1);

    // transition all mipmap levels to shaderReadOnlyOptimal
    vk::ImageMemoryBarrier transition = transitionImage(src,
                                                        vk::ImageLayout::eTransferSrcOptimal,
                                                        vk::ImageLayout::eShaderReadOnlyOptimal,
                                                        range);

    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                        vk::PipelineStageFlagBits::eTopOfPipe,
                        vk::DependencyFlagBits::eByRegion,
                        nullptr, nullptr, transition);

    cmd.end();

    vk::SubmitInfo submit(0, nullptr, nullptr, 1, &cmd, 0, nullptr);
    mQueue->submit({submit}, (*mFences)[*mIndex]);
    *mIndex = (*mIndex + 1) % (*mFences).size();
}

void BufferImageTransferer::flush() {
    *mIndex = 0;
    mDevice->waitForFences(std::vector<vk::Fence>(mFences->begin(), mFences->end()), true, UINT64_MAX);
}

vk::ImageMemoryBarrier
BufferImageTransferer::transitionImage(Image image,
                                       vk::ImageLayout oldLayout,
                                       vk::ImageLayout newLayout,
                                       vk::ImageSubresourceRange imageSubResourceRange) {
    vk::AccessFlags src, dst;

    if(oldLayout == vk::ImageLayout::eUndefined)
        src = vk::AccessFlags();

    // If it was preinitialized: the host write on it
    else if(oldLayout == vk::ImageLayout::ePreinitialized)
        src = vk::AccessFlagBits::eHostWrite;

    // If it was in transferDst, we wait for transferWrite
    else if(oldLayout == vk::ImageLayout::eTransferDstOptimal)
        src = vk::AccessFlagBits::eTransferWrite;

    // If it was in transferSrc, we wait for transferRead
    else if(oldLayout == vk::ImageLayout::eTransferSrcOptimal)
        src = vk::AccessFlagBits::eTransferRead;

    else
        assert(!"This oldLayout is not managed");

    // If it will be transferSrcOptimal, it should be ready at transferRead
    if(newLayout == vk::ImageLayout::eTransferSrcOptimal)
        dst = vk::AccessFlagBits::eTransferRead;

    // If it will be transferDstOptimal, it should be ready at transferWrite
    else if(newLayout == vk::ImageLayout::eTransferDstOptimal)
        dst = vk::AccessFlagBits::eTransferWrite;

    // If it will be shaderReadOnlyOptimal, it should be ready at shaderRead
    else if(newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        dst = vk::AccessFlagBits::eShaderRead;

    else
        assert(!"This newLayout is not managed");

    return vk::ImageMemoryBarrier(src, dst, oldLayout, newLayout, 0, 0,
                                  image, imageSubResourceRange);
}
