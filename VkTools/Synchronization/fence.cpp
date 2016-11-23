#include "fence.hpp"

Fence::Fence(const Device &device, bool signaled) :
    VkResource(device) {
    vk::FenceCreateInfo info;

    if(signaled)
        info.flags = vk::FenceCreateFlagBits::eSignaled;

    m_fence = device.createFence(info);
}

Fence &Fence::operator =(Fence fence) {
    using std::swap;
    swap(static_cast<VkResource&>(fence), static_cast<VkResource&>(*this));
    swap(static_cast<vk::Fence&>(fence), static_cast<vk::Fence&>(*this));
    return *this;
}

void Fence::wait() {
    mDevice->waitForFences({m_fence}, true, UINT64_MAX);
}

void Fence::reset() {
    mDevice->resetFences({m_fence});
}

Fence::~Fence() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyFence(m_fence);
}
