#pragma once
#include "device.hpp"
#include "vulkan/vkresource.hpp"

class FrameBuffer : public VkResource, public vk::Framebuffer
{
public:
    FrameBuffer(vk::Device &device, vk::FramebufferCreateInfo const &createInfo);

    friend void swap(FrameBuffer &f1, FrameBuffer &b2);
    FrameBuffer(FrameBuffer &&frameBuffer);
    FrameBuffer(FrameBuffer const &frameBuffer);
    FrameBuffer &operator=(FrameBuffer frameBuffer);

    ~FrameBuffer();
};
