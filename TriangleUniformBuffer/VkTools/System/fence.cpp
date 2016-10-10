#include "fence.hpp"

void swap(Fence &f1, Fence &f2) {
    using std::swap;
    swap(static_cast<VkResource&>(f1), static_cast<VkResource&>(f2));
    swap(f1.m_fence, f2.m_fence);
}

Fence::Fence(Fence &&fence) :
    VkResource(std::move(fence)),
    vk::Fence(fence) {
    swap(*this, fence);
}

Fence::Fence(const Fence &fence) :
    VkResource(fence),
    vk::Fence(fence) {

}

Fence &Fence::operator =(Fence fence) {
    swap(fence, *this);
    return *this;
}

Fence::Fence(vk::Device &device, bool signaled) :
    VkResource(device) {
    vk::FenceCreateInfo info;

    if(signaled)
        info.flags = vk::FenceCreateFlagBits::eSignaled;

    m_fence = device.createFence(info);
}

void Fence::wait() {
    mDevice.waitForFences({m_fence}, true, UINT64_MAX);
}

void Fence::reset() {
    mDevice.resetFences({m_fence});
}

Fence::~Fence() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice.destroyFence(m_fence);
}
