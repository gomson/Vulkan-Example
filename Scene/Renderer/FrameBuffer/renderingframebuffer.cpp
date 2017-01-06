#include "renderingframebuffer.hpp"

RenderingFrameBuffer::RenderingFrameBuffer(const Device &device, uint32_t width, uint32_t height,
                                         const RenderingPass &geometryRenderPass, std::shared_ptr<AbstractAllocator> allocator) :
    CompleteFrameBuffer(device, width, height) {
    vk::ImageCreateInfo diffuseInfo(vk::ImageCreateFlags(),
                                  vk::ImageType::e2D,
                                  vk::Format::eR8G8B8A8Unorm,
                                  vk::Extent3D(width, height, 1),
                                  1, 1,
                                  vk::SampleCountFlagBits::e1,
                                  vk::ImageTiling::eOptimal,
                                  vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                                  vk::SharingMode::eExclusive,
                                  0,
                                  nullptr,
                                  vk::ImageLayout::eUndefined);

    vk::ImageViewCreateInfo diffuseViewInfo(vk::ImageViewCreateFlags(),
                                            vk::Image(),
                                            vk::ImageViewType::e2D,
                                            vk::Format::eR8G8B8A8Unorm,
                                            vk::ComponentMapping(),
                                            vk::ImageSubresourceRange(
                                                vk::ImageAspectFlagBits::eColor,
                                                0, 1, 0, 1));

    vk::ImageCreateInfo depthInfo(diffuseInfo);

    vk::ImageViewCreateInfo depthViewInfo(diffuseViewInfo);

    depthInfo.format = vk::Format::eD24UnormS8Uint;
    depthInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;

    depthViewInfo.format = depthInfo.format;

    depthViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

    mImages->emplace_back(device, diffuseInfo, allocator);
    mImages->emplace_back(device, depthInfo, allocator);

    diffuseViewInfo.image = (*mImages)[0];
    depthViewInfo.image = (*mImages)[1];

    mImageViews->emplace_back(device, diffuseViewInfo);
    mImageViews->emplace_back(device, depthViewInfo);

    vk::ImageView views[] = {(*mImageViews)[0], (*mImageViews)[1]};

    vk::FramebufferCreateInfo info(vk::FramebufferCreateFlags(),
                                   geometryRenderPass, 2, views,
                                   width, height, 1);
    m_framebuffer = device.createFramebuffer(info);

}
