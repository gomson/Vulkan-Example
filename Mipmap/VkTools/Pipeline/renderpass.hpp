#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class RenderPass : public VkResource, public vk::RenderPass
{
protected:
    RenderPass(Device &device);

public:
    friend void swap(RenderPass &r1, RenderPass &r2);
    RenderPass(RenderPass &&renderPass);
    RenderPass(RenderPass const &renderPass);
    RenderPass &operator=(RenderPass renderPass);

public:
    ~RenderPass();
};
