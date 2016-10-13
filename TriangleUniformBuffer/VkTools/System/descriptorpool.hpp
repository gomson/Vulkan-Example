#pragma once
#include "vulkan/vkresource.hpp"

class DescriptorPool : public VkResource, public vk::DescriptorPool
{
public:
    DescriptorPool(Device &device, uint32_t maxSet,
                   const vk::ArrayProxy<vk::DescriptorPoolSize> &poolSizes);

    friend void swap(DescriptorPool &p1, DescriptorPool &p2);
    DescriptorPool(DescriptorPool &&descriptorPool);
    DescriptorPool(DescriptorPool const &descriptorPool);
    DescriptorPool &operator=(DescriptorPool descriptorPool);

    ~DescriptorPool();
private:
};
