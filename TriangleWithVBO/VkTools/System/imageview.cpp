#include "imageview.hpp"

ImageView::ImageView(vk::Device &device, const vk::ImageViewCreateInfo &info) :
    VkResource(device)
{
    m_imageView = device.createImageView(info);
}

ImageView::~ImageView() {
    mDevice.destroyImageView(m_imageView);
}
