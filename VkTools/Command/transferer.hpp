#pragma once
#include "../Command/commandbuffersubmitter.hpp"

class Transferer : public ObserverCommandBufferSubmitter
{
public:
    Transferer(Device const &device, uint32_t numberBuffers, vk::DeviceSize sizeTransfererBuffers,
               std::shared_ptr<AbstractAllocator> allocator, CommandBufferSubmitter &commandBufferSubmitter);

    void cacheResource(std::shared_ptr<VkResource> resource);

    void transfer(const Buffer &src, Buffer &dst, vk::BufferCopy bufferCopy);

    void transfer(Buffer &buffer, vk::DeviceSize offset, vk::DeviceSize size, void *data);

    void transfer(Buffer &buffer, Image &image, vk::ImageLayout initialLayout,
                  vk::ImageLayout finalLayout, vk::BufferImageCopy bufferImageCopy);

    void transfer(Image const &src, Image &dst,
                  vk::ImageLayout oldSrcLayout, vk::ImageLayout oldDstLayout,
                  vk::ImageLayout newSrcLayout, vk::ImageLayout newDstLayout,
                  vk::ImageSubresourceRange srcImageSubResourceRange,
                  vk::ImageSubresourceRange dstImageSubResourceRange,
                  vk::ImageSubresourceLayers srcSubResource,
                  vk::ImageSubresourceLayers dstSubResource,
                  vk::Offset3D srcOffset,
                  vk::Offset3D dstOffset,
                  vk::Extent3D extent);

    void buildMipMap(Image &src);

    std::shared_ptr<AbstractAllocator> getAllocator();

    void notify();

private:
    std::shared_ptr<AbstractAllocator> mAllocator;
    std::shared_ptr<CommandBufferSubmitter> mCommandBufferSubmitter;
    std::shared_ptr<std::vector<Buffer>> mTransfererBuffers = std::make_shared<std::vector<Buffer>>();
    std::shared_ptr<uint32_t> mSizeTransfererBuffers;
    std::shared_ptr<uint32_t> mIndex = std::make_shared<uint32_t>(0);
    std::shared_ptr<std::vector<vk::DeviceSize>> mSizeAlreadyUsed = std::make_shared<std::vector<vk::DeviceSize>>();

    void addTransferBuffer();

    vk::ImageMemoryBarrier transitionImage(Image image,
                                           vk::ImageLayout oldLayout,
                                           vk::ImageLayout newLayout,
                                           vk::ImageSubresourceRange imageSubResourceRange);
};
