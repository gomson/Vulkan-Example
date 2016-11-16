#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class ImageView : public VkResource, public vk::ImageView
{
public:
    ImageView() = default;
    ImageView(const Device &device, vk::ImageViewCreateInfo const &info);

    ImageView(ImageView &&imageView) = default;
    ImageView(ImageView const &imageView) = default;
    ImageView &operator=(ImageView imageView);

    ~ImageView();
};
