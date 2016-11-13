#pragma once
#include "device.hpp"
#include "vulkan/vkresource.hpp"

class FrameBuffer : public VkResource, public vk::Framebuffer
{
public:
    FrameBuffer(Device &device, vk::FramebufferCreateInfo const &createInfo);

    FrameBuffer(FrameBuffer &&frameBuffer) = default;
    FrameBuffer(FrameBuffer const &frameBuffer) = default;
    FrameBuffer &operator=(FrameBuffer const &frameBuffer) = default;

    ~FrameBuffer();
};
