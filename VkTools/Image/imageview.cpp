#include "imageview.hpp"

ImageView::ImageView(Device const &device, const vk::ImageViewCreateInfo &info) :
    VkResource(device)
{
    m_imageView = device.createImageView(info);
}

ImageView &ImageView::operator =(ImageView imageView) {
    using std::swap;
    swap(static_cast<VkResource&>(imageView), static_cast<VkResource&>(*this));
    swap(static_cast<vk::ImageView&>(imageView), static_cast<vk::ImageView&>(*this));
    return *this;
}

ImageView::~ImageView() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyImageView(m_imageView);
}
