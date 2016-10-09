#include "commandpool.hpp"

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
    mDevice.destroyCommandPool(m_commandPool);
}
