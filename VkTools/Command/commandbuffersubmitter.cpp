#include "commandbuffersubmitter.hpp"

CommandBufferSubmitter::CommandBufferSubmitter(Device &device, uint32_t numberCommandBuffers) :
    mDevice(std::make_shared<Device>(device)),
    mQueue(std::make_shared<vk::Queue>(device.getTransferQueue())),
    mCommandPool(std::make_shared<CommandPool>(device, true, true, device.getIndexTransferQueue())) {
    mFences->emplace_back(Fence(device, false));
    mCommandBuffers->emplace_back(mCommandPool->allocate(vk::CommandBufferLevel::ePrimary, numberCommandBuffers));
}

void CommandBufferSubmitter::addObserver(ObserverCommandBufferSubmitter *observer) {
    mObservers->emplace_back(observer);
}

vk::CommandBuffer CommandBufferSubmitter::createCommandBuffer() {
    if(*mCommandBufferIndex >= (*mCommandBuffers)[*mSubmitIndex].size()) {
        auto buffers = mCommandPool->allocate(vk::CommandBufferLevel::ePrimary, 10);

        for(auto &b : buffers)
            (*mCommandBuffers)[*mSubmitIndex].emplace_back(b);
    }

    return (*mCommandBuffers)[*mSubmitIndex][(*mCommandBufferIndex)++];
}

void CommandBufferSubmitter::submit() {
    vk::SubmitInfo info;
    info.setCommandBufferCount(*mCommandBufferIndex).setPCommandBuffers((*mCommandBuffers)[*mSubmitIndex].data());
    mQueue->submit(info, (*mFences)[*mSubmitIndex]);
    mFences->emplace_back(Fence(*mDevice, false));
    mCommandBuffers->resize(mCommandBuffers->size() + 1);
    *mCommandBufferIndex = 0;
    ++*mSubmitIndex;
}

void CommandBufferSubmitter::submit(bool wait) {
    submit();
    if(wait)
        this->wait(*mSubmitIndex - 1);
}

void CommandBufferSubmitter::wait(unsigned i) {
    (*mFences)[i].wait();
    (*mFences)[i].reset();

    for(auto &observer : *mObservers)
        observer->notify();

    if(*mSubmitIndex > 0) {
        mDevice->freeCommandBuffers(*mCommandPool, (*mCommandBuffers)[i]);
        mCommandBuffers->erase(mCommandBuffers->begin() + i);
        mFences->erase(mFences->begin() + i);

        if(i <= *mSubmitIndex)
            --*mSubmitIndex;
    }
}

void CommandBufferSubmitter::waitAll() {
    while(*mSubmitIndex)
        wait(*mSubmitIndex - 1);
}
