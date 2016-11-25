#pragma once
#include "vulkan/vkresource.hpp"

class DescriptorSetLayout : public VkResource, public vk::DescriptorSetLayout
{
public:
    DescriptorSetLayout(const Device &device, vk::DescriptorSetLayoutCreateInfo info);

    DescriptorSetLayout(DescriptorSetLayout &&descriptorSetLayout) = default;
    DescriptorSetLayout(DescriptorSetLayout const &descriptorSetLayout) = default;
    DescriptorSetLayout &operator=(DescriptorSetLayout descriptorSetLayout);

    ~DescriptorSetLayout();
};
