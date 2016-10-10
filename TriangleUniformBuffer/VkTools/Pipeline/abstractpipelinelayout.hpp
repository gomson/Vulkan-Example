#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class AbstractPipelineLayout : public VkResource, public vk::PipelineLayout, private NotCopyable
{
protected:
    AbstractPipelineLayout(Device &device);

public:
    std::vector<vk::DescriptorSetLayout> const &getDescriptorSetLayouts() const;
    std::vector<vk::DescriptorSet> const &getDescriptorSet() const;

    ~AbstractPipelineLayout();

protected:
    std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
    std::vector<vk::DescriptorSet> mDescriptorSet;
};
