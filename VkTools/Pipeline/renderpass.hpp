#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class RenderPass : public VkResource, public vk::RenderPass
{
protected:
    RenderPass(Device &device);

public:
    RenderPass(RenderPass &&renderPass) = default;
    RenderPass(RenderPass const &renderPass) = default;
    RenderPass &operator=(RenderPass const &renderPass) = default;

public:
    ~RenderPass();
};
