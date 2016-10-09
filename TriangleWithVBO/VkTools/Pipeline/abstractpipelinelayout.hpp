#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class AbstractPipelineLayout : public vk::PipelineLayout, public VkResource
{
protected:
    AbstractPipelineLayout(Device &device);

public:
    ~AbstractPipelineLayout();
};
