#include "geometryframebuffer.hpp"

GeometryFrameBuffer::GeometryFrameBuffer(const Device &device, uint32_t width, uint32_t height,
                                         const GeometryRenderPass &geometryRenderPass, std::shared_ptr<AbstractAllocator> allocator) :
    mAllocator(allocator) {

    vk::ImageCreateInfo diffuseInfo(vk::ImageCreateFlags(),
                                  vk::ImageType::e2D,
                                  vk::Format::eR8G8B8A8Unorm,
                                  vk::Extent3D(width, height, 1),
                                  1, 1,
                                  vk::SampleCountFlagBits::e1,
                                  vk::ImageTiling::eOptimal,
                                  vk::ImageUsageFlagBits::eColorAttachment,
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
    vk::ImageCreateInfo normalInfo(diffuseInfo);
    vk::ImageViewCreateInfo depthViewInfo(diffuseViewInfo);
    vk::ImageViewCreateInfo normalViewInfo(diffuseViewInfo);

    depthInfo.format = vk::Format::eD24UnormS8Uint;
    normalInfo.format = vk::Format::eR16G16B16A16Sfloat;

    depthInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;

    depthViewInfo.format = depthInfo.format;
    normalViewInfo.format = normalInfo.format;

    depthViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

    mImages->emplace_back(device, diffuseInfo, mAllocator);
    mImages->emplace_back(device, normalInfo, mAllocator);
    mImages->emplace_back(device, depthInfo, mAllocator);

    diffuseViewInfo.image = (*mImages)[0];
    normalViewInfo.image = (*mImages)[1];
    depthViewInfo.image = (*mImages)[2];

    mImageViews->emplace_back(device, diffuseViewInfo);
    mImageViews->emplace_back(device, normalViewInfo);
    mImageViews->emplace_back(device, depthViewInfo);

    vk::ImageView views[] = {(*mImageViews)[0], (*mImageViews)[1], (*mImageViews)[2]};

    vk::FramebufferCreateInfo info(vk::FramebufferCreateFlags(),
                                   geometryRenderPass, 3, views,
                                   width, height, 1);
    m_framebuffer = device.createFramebuffer(info);

}
