#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class AbstractRenderPass : public vk::RenderPass, public VkResource, private NotCopyable
{
protected:
    AbstractRenderPass(Device &device);

public:
    ~AbstractRenderPass();
};
