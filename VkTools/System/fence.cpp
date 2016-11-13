#include "fence.hpp"

Fence::Fence(Device &device, bool signaled) :
    VkResource(device) {
    vk::FenceCreateInfo info;

    if(signaled)
        info.flags = vk::FenceCreateFlagBits::eSignaled;

    m_fence = device.createFence(info);
}

void Fence::wait() {
    mDevice->waitForFences({m_fence}, true, UINT64_MAX);
}

void Fence::reset() {
    mDevice->resetFences({m_fence});
}

Fence::~Fence() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyFence(m_fence);
}
