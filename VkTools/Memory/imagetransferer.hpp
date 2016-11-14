#pragma once
#include "../System/commandbuffersubmitter.hpp"
#include "image.hpp"

class ImageTransferer
{
public:
    ImageTransferer(Device &device, CommandBufferSubmitter commandBufferSubmitter);

    ImageTransferer(ImageTransferer &&bufferImageTransferer) = default;
    ImageTransferer(ImageTransferer const &bufferImageTransferer) = default;
    ImageTransferer &operator=(ImageTransferer const &bufferImageTransferer) = default;

    void transfer(const Image &src, Image &dst,
                  vk::ImageLayout oldSrcLayout, vk::ImageLayout oldDstLayout,
                  vk::ImageLayout newSrcLayout, vk::ImageLayout newDstLayout,
                  vk::ImageSubresourceRange srcImageSubResourceRange,
                  vk::ImageSubresourceRange dstImageSubResourceRange,
                  vk::ImageSubresourceLayers srcSubResource,
                  vk::ImageSubresourceLayers dstSubResource,
                  vk::Offset3D srcOffset, vk::Offset3D dstOffset,
                  vk::Extent3D extent);

    void buildMipMap(Image &src);

    std::shared_ptr<CommandBufferSubmitter> getCommandBufferSubmitter();

private:
    std::shared_ptr<CommandBufferSubmitter> mCommandBufferSubmitter;

    vk::ImageMemoryBarrier transitionImage(Image image,
                                           vk::ImageLayout oldLayout,
                                           vk::ImageLayout newLayout,
                                           vk::ImageSubresourceRange imageSubResourceRange);
};
