#include "framebuffer.hpp"
/*
void swap(FrameBuffer &f1, FrameBuffer &f2) {
    using std::swap;
    swap(static_cast<VkResource&>(f1), static_cast<VkResource&>(f2));
    swap(f1.m_framebuffer, f2.m_framebuffer);
}

FrameBuffer::FrameBuffer(FrameBuffer &&frameBuffer) :
    VkResource(frameBuffer),
    vk::Framebuffer(frameBuffer) {
    swap(*this, frameBuffer);
}

FrameBuffer::FrameBuffer(const FrameBuffer &frameBuffer) :
    VkResource(std::move(frameBuffer)),
    vk::Framebuffer(frameBuffer) {

}

FrameBuffer &FrameBuffer::operator =(FrameBuffer frameBuffer) {
    swap(*this, frameBuffer);
    return *this;
}*/

FrameBuffer::FrameBuffer(const Device &device, vk::FramebufferCreateInfo const &createInfo) :
    VkResource(device) {
    m_framebuffer = device.createFramebuffer(createInfo);

}

FrameBuffer::FrameBuffer(const Device &device) :
    VkResource(device) {

}

FrameBuffer &FrameBuffer::operator=(FrameBuffer framebuffer) {
    using std::swap;
    swap(static_cast<VkResource&>(framebuffer), static_cast<VkResource&>(*this));
    swap(static_cast<vk::Framebuffer&>(framebuffer), static_cast<vk::Framebuffer&>(*this));

    return *this;
}

FrameBuffer::~FrameBuffer() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyFramebuffer(m_framebuffer);
}
