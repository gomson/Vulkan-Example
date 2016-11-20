#include "commandbuffersubmitter.hpp"

CommandBufferSubmitter::CommandBufferSubmitter(Device &device, CommandPool &commandPool, uint32_t numberCommandBuffers) :
    mDevice(std::make_shared<Device>(device)),
    mQueue(std::make_shared<vk::Queue>(device.getTransferQueue())),
    mCommandPool(std::make_shared<CommandPool>(commandPool)),
    mNumberCommandBufferToAllocate(std::make_shared<uint32_t>(numberCommandBuffers)) {
    addSubmitIndex();
}

void CommandBufferSubmitter::addSubmitIndex() {
    mFences->emplace_back(Fence(*mDevice, false));
    mCommandBuffers->resize(mCommandBuffers->size() + 1);
    mTemporaryResources->resize(mTemporaryResources->size() + 1);
    mObservers->resize(mObservers->size() + 1);
    *mCommandBufferIndex = 0;
    if(mFences->size() != 1)
        ++*mSubmitIndex;
}

void CommandBufferSubmitter::destroySubmit(uint32_t i) {
    if(*mSubmitIndex > 0) {
        mDevice->freeCommandBuffers(*mCommandPool, (*mCommandBuffers)[i]);
        mCommandBuffers->erase(mCommandBuffers->begin() + i);
        mFences->erase(mFences->begin() + i);
        mTemporaryResources->erase(mTemporaryResources->begin() + i);
        mObservers->erase(mObservers->begin() + i);

        if(i <= *mSubmitIndex)
            --*mSubmitIndex;
    }
}

void CommandBufferSubmitter::cacheResource(std::shared_ptr<VkResource> &resource) {
    (*mTemporaryResources)[*mSubmitIndex].emplace_back(resource);
}

vk::CommandBuffer CommandBufferSubmitter::createCommandBuffer(ObserverCommandBufferSubmitter *observer) {
    if(*mCommandBufferIndex >= (*mCommandBuffers)[*mSubmitIndex].size()) {
        auto buffers = mCommandPool->allocate(vk::CommandBufferLevel::ePrimary, *mNumberCommandBufferToAllocate);

        for(auto &b : buffers)
            (*mCommandBuffers)[*mSubmitIndex].emplace_back(b);
    }

    if(observer != nullptr)
        (*mObservers)[*mSubmitIndex].insert(observer);
    return (*mCommandBuffers)[*mSubmitIndex][(*mCommandBufferIndex)++];
}

void CommandBufferSubmitter::submit() {
    vk::SubmitInfo info;
    info.setCommandBufferCount(*mCommandBufferIndex).setPCommandBuffers((*mCommandBuffers)[*mSubmitIndex].data());
    mQueue->submit(info, (*mFences)[*mSubmitIndex]);
    addSubmitIndex();
}

void CommandBufferSubmitter::submit(bool wait) {
    submit();
    if(wait)
        this->wait(*mSubmitIndex - 1);
}

void CommandBufferSubmitter::wait(unsigned i) {
    (*mFences)[i].wait();
    (*mFences)[i].reset();

    for(auto &observer : (*mObservers)[i])
        observer->notify();
    destroySubmit(i);
}

void CommandBufferSubmitter::waitAll() {
    while(*mSubmitIndex)
        wait(*mSubmitIndex - 1);
}
