#pragma once
#include "vulkan/vkresource.hpp"

class DescriptorPool : public VkResource, public vk::DescriptorPool
{
public:
    DescriptorPool() = default;
    DescriptorPool(Device const &device, uint32_t maxSet,
                   vk::ArrayProxy<vk::DescriptorPoolSize> poolSizes);

    DescriptorPool(DescriptorPool &&descriptorPool) = default;
    DescriptorPool(DescriptorPool const &descriptorPool) = default;
    DescriptorPool &operator=(DescriptorPool descriptorPool);

    ~DescriptorPool();
private:
};
