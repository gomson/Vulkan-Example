#include "imageview.hpp"

void swap(ImageView &i1, ImageView &i2) {
    using std::swap;
    swap(static_cast<VkResource&>(i1), static_cast<VkResource&>(i2));
    swap(i1.m_imageView, i2.m_imageView);
}

ImageView::ImageView(ImageView &&imageView) :
    VkResource(imageView),
    vk::ImageView(imageView) {
    swap(*this, imageView);
}

ImageView::ImageView(const ImageView &imageView) :
    VkResource(std::move(imageView)),
    vk::ImageView(imageView) {

}

ImageView &ImageView::operator=(ImageView imageView) {
    swap(*this, imageView);
    return *this;
}

ImageView::ImageView(Device const &device, const vk::ImageViewCreateInfo &info) :
    VkResource(device)
{
    m_imageView = device.createImageView(info);
}

ImageView::~ImageView() {
    if((VkDevice)mDevice != VK_NULL_HANDLE && mCount != nullptr && --(*mCount) == 0)
        mDevice.destroyImageView(m_imageView);
}
