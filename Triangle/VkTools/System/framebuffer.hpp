#pragma once
#include "device.hpp"
#include "vulkan/vkresource.hpp"

class FrameBuffer : public vk::Framebuffer, public VkResource
{
public:
    FrameBuffer(vk::Device &device, vk::FramebufferCreateInfo const &createInfo);

    ~FrameBuffer();
};
