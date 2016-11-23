#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class FrameBuffer : public VkResource, public vk::Framebuffer
{
public:
    FrameBuffer() = default;
    FrameBuffer(Device const &device, vk::FramebufferCreateInfo const &createInfo);

    FrameBuffer(FrameBuffer &&frameBuffer) = default;
    FrameBuffer(FrameBuffer const &frameBuffer) = default;
    FrameBuffer &operator=(FrameBuffer frameBuffer);

    ~FrameBuffer();
};
