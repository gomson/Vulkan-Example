#pragma once
#include "vulkan.hpp"

class VkResource
{
protected:
    VkResource() = default;
    VkResource(vk::Device device);
    
    friend void swap(VkResource &value1, VkResource &value2);
    VkResource(VkResource const &resource);
    VkResource(VkResource &&resource);

    virtual ~VkResource() = 0;

protected:
    vk::Device mDevice;
    std::shared_ptr<uint32_t> mCount = nullptr;
};
