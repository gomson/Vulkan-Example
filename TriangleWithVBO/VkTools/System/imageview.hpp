#pragma once
#include "device.hpp"
#include "vulkan/vkresource.hpp"

class ImageView : public vk::ImageView, public VkResource
{
public:
    ImageView(vk::Device &device, vk::ImageViewCreateInfo const &info);

    ~ImageView();
};
