#include "commandbuffersubmitter.hpp"

CommandBufferSubmitter::CommandBufferSubmitter(Device &device, uint32_t numberCommandBuffers) :
    mDevice(std::make_shared<Device>(device)),
    mQueue(std::make_shared<vk::Queue>(device.getTransferQueue())),
    mCommandPool(std::make_shared<CommandPool>(device, true, true, device.getIndexTransferQueue())),
    mFence(std::make_shared<Fence>(device, false)) {
    *mCommandBuffers = mCommandPool->allocate(vk::CommandBufferLevel::ePrimary, numberCommandBuffers);
}

void CommandBufferSubmitter::addObserver(ObserverCommandBufferSubmitter *observer) {
    mObservers->emplace_back(observer);
}

vk::CommandBuffer CommandBufferSubmitter::createCommandBuffer() {
    if(*mIndex >= mCommandBuffers->size()) {
        auto buffers = mCommandPool->allocate(vk::CommandBufferLevel::ePrimary, 10);

        for(auto &b : buffers)
            mCommandBuffers->emplace_back(b);
    }

    return (*mCommandBuffers)[(*mIndex)++];
}

void CommandBufferSubmitter::submit() {
    vk::SubmitInfo info;
    info.setCommandBufferCount(*mIndex).setPCommandBuffers(mCommandBuffers->data());
    mFence->reset();
    mQueue->submit(info, *mFence);
}

void CommandBufferSubmitter::submit(bool wait) {
    vk::SubmitInfo info;
    info.setCommandBufferCount(*mIndex).setPCommandBuffers(mCommandBuffers->data());
    mFence->reset();
    mQueue->submit(info, *mFence);

    if(wait)
        this->wait();
}

void CommandBufferSubmitter::wait() {
    *mIndex = 0;
    mFence->wait();
    mFence->reset();
    for(auto &observer : *mObservers)
        observer->notify();
}
