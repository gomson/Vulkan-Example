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

    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
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

void BufferImageTransferer::flush() {
    mIndex = 0;
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

    else if(oldLayout == vk::ImageLayout::ePreinitialized)
        src = vk::AccessFlagBits::eHostWrite;

    else if(oldLayout == vk::ImageLayout::eTransferDstOptimal)
        src = vk::AccessFlagBits::eTransferWrite;

    else if(oldLayout == vk::ImageLayout::eTransferSrcOptimal)
        src = vk::AccessFlagBits::eTransferRead;

    else
        assert(!"This oldLayout is not managed");

    if(newLayout == vk::ImageLayout::eTransferSrcOptimal)
        dst = vk::AccessFlagBits::eTransferRead;

    else if(newLayout == vk::ImageLayout::eTransferDstOptimal)
        dst = vk::AccessFlagBits::eTransferWrite;

    else if(newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        dst = vk::AccessFlagBits::eShaderRead;

    else
        assert(!"This newLayout is not managed");

    return vk::ImageMemoryBarrier(src, dst, oldLayout, newLayout, 0, 0,
                                  image, imageSubResourceRange);
}
