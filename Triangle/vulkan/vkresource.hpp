#pragma once
#include "vulkan.hpp"

class VkResource : private NotCopyable
{
public:
    VkResource(vk::Device device);

    virtual ~VkResource() = 0;

protected:
    vk::Device mDevice;
};
