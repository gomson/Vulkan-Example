#pragma once
#include "device.hpp"
#include "vulkan/vkresource.hpp"

class ImageView : public VkResource, public vk::ImageView
{
public:
    ImageView(vk::Device &device, vk::ImageViewCreateInfo const &info);

    friend void swap(ImageView &i1, ImageView &i2);
    ImageView(ImageView &&imageView);
    ImageView(ImageView const &imageView);
    ImageView &operator=(ImageView imageView);

    ~ImageView();
};
