#pragma once
#include "buffer.hpp"
#include "image.hpp"
#include "VkTools/System/commandpool.hpp"
#include "VkTools/System/fence.hpp"

class BufferImageTransferer
{
public:
    BufferImageTransferer(Device &device, uint32_t numberCommandBuffer);

    BufferImageTransferer(BufferImageTransferer &&bufferImageTransferer) = default;
    BufferImageTransferer(BufferImageTransferer const &bufferImageTransferer) = default;
    BufferImageTransferer &operator=(BufferImageTransferer const &bufferImageTransferer) = default;

    void transfer(const Buffer &src, Buffer &dst,
                  vk::DeviceSize offsetSrc,
                  vk::DeviceSize offsetDst,
                  vk::DeviceSize size);

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

    void flush();

private:
    std::shared_ptr<Device> mDevice = std::make_shared<Device>();
    std::shared_ptr<CommandPool> mCommandPool;
    std::shared_ptr<std::vector<vk::CommandBuffer>> mCommandBuffer = std::make_shared<std::vector<vk::CommandBuffer>>();
    std::shared_ptr<std::vector<Fence>> mFences = std::make_shared<std::vector<Fence>>();
    std::shared_ptr<vk::Queue> mQueue = std::make_shared<vk::Queue>();
    std::shared_ptr<uint32_t> mIndex = std::make_shared<uint32_t>(0);

    vk::ImageMemoryBarrier transitionImage(Image image,
                                           vk::ImageLayout oldLayout,
                                           vk::ImageLayout newLayout,
                                           vk::ImageSubresourceRange imageSubResourceRange);
};
