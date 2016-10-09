#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class AbstractPipeline : public vk::Pipeline, public VkResource
{
protected:
    AbstractPipeline(Device &device);

public:
    ~AbstractPipeline();
};
