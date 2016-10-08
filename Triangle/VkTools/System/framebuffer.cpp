#include "framebuffer.hpp"

FrameBuffer::FrameBuffer(vk::Device &device, vk::FramebufferCreateInfo const &createInfo) :
    VkResource(device) {
    m_framebuffer = device.createFramebuffer(createInfo);

}

FrameBuffer::~FrameBuffer() {
    mDevice.destroyFramebuffer(m_framebuffer);
}
