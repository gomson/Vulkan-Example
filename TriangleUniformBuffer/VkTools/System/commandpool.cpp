#include "commandpool.hpp"

void swap(CommandPool &p1, CommandPool &p2) {
    using std::swap;
    swap(static_cast<VkResource&>(p1), static_cast<VkResource&>(p2));
    swap(p1.m_commandPool, p2.m_commandPool);
}

CommandPool::CommandPool(CommandPool &&commandPool) :
    VkResource(std::move(commandPool)),
    vk::CommandPool(commandPool) {
    swap(*this, commandPool);
}

CommandPool::CommandPool(CommandPool const &commandPool) :
    VkResource(commandPool),
    vk::CommandPool(commandPool) {

}

CommandPool &CommandPool::operator=(CommandPool commandPool) {
    swap(*this, commandPool);
    return *this;
}

CommandPool::CommandPool(vk::Device &device, bool transient, bool resetableBuffer,
                         uint32_t queueFamillyIndex) :
    VkResource(device) {
    vk::CommandPoolCreateInfo info(vk::CommandPoolCreateFlags(), queueFamillyIndex);

    if(transient)
        info.flags |= vk::CommandPoolCreateFlagBits::eTransient;

    if(resetableBuffer)
        info.flags |= vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    m_commandPool = device.createCommandPool(info);
}

std::vector<vk::CommandBuffer> CommandPool::allocate(vk::CommandBufferLevel level, uint32_t count) {
    vk::CommandBufferAllocateInfo info(m_commandPool, level, count);
    return mDevice.allocateCommandBuffers(info);
}

void CommandPool::reset(bool releaseResources) {
    vk::CommandPoolResetFlags flags;
    if(releaseResources)
        flags |= vk::CommandPoolResetFlagBits::eReleaseResources;
    mDevice.resetCommandPool(m_commandPool, flags);
}

CommandPool::~CommandPool() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice.destroyCommandPool(m_commandPool);
}
