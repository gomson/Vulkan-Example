#include "commandbuffersubmitter.hpp"
#include <iterator>

CommandBufferSubmitter::CommandBufferSubmitter(const Device &device, CommandPool &commandPool, uint32_t numberCommandBuffers) :
    mDevice(std::make_shared<Device>(device)),
    mQueue(std::make_shared<vk::Queue>(device.getTransferQueue())),
    mCommandPool(std::make_shared<CommandPool>(commandPool)),
    mNumberCommandBufferToAllocate(std::make_shared<uint32_t>(numberCommandBuffers)) {
}

void CommandBufferSubmitter::setNumberBatch(uint32_t numberBatch) {
    mCommandBuffers->resize(numberBatch);
    mFences->resize(numberBatch);

    for(auto &fence : *mFences)
        fence = Fence(*mDevice, true);

    mTemporaryResources->resize(numberBatch);
    mObservers->resize(numberBatch);
}

void CommandBufferSubmitter::setCurrentBatch(uint32_t i) {
    assert(i <= mFences->size());
    *mCurrentBatch = i;
}

void CommandBufferSubmitter::cacheResource(std::shared_ptr<VkResource> &resource) {
    (*mTemporaryResources)[*mCurrentBatch].emplace_back(resource);
}

vk::CommandBuffer CommandBufferSubmitter::createCommandBuffer(ObserverCommandBufferSubmitter *observer) {
    if(mCommandBuffers->empty())
        setNumberBatch(1);
    if(*mCommandBufferIndex >= (*mCommandBuffers)[*mCurrentBatch].size()) {
        auto buffers = mCommandPool->allocate(vk::CommandBufferLevel::ePrimary, *mNumberCommandBufferToAllocate);

        std::copy(buffers.begin(), buffers.end(), std::back_inserter((*mCommandBuffers)[*mCurrentBatch]));
    }

    if(observer != nullptr)
        (*mObservers)[*mCurrentBatch].insert(observer);
    return (*mCommandBuffers)[*mCurrentBatch][(*mCommandBufferIndex)++];
}

void CommandBufferSubmitter::submit(uint32_t waitSemaphoreCount, const vk::Semaphore *waitSemaphores, vk::PipelineStageFlags *waitDstStageMask, uint32_t signalSemaphoreCount, const vk::Semaphore *signalSemaphores) {
    vk::SubmitInfo info(waitSemaphoreCount, waitSemaphores,
                        waitDstStageMask,
                        *mCommandBufferIndex,
                        (*mCommandBuffers)[*mCurrentBatch].data(),
                        signalSemaphoreCount, signalSemaphores);

    mQueue->submit(info, (*mFences)[*mCurrentBatch]);
    *mCommandBufferIndex = 0;
}

void CommandBufferSubmitter::submitWithFullSynchro() {
    vk::SubmitInfo info;

    info.commandBufferCount = *mCommandBufferIndex;
    info.pCommandBuffers = (*mCommandBuffers)[*mCurrentBatch].data();
    mQueue->submit(info, vk::Fence());
    *mCommandBufferIndex = 0;
    mDevice->waitIdle();
}

void CommandBufferSubmitter::wait() {
    (*mFences)[*mCurrentBatch].wait();
    (*mFences)[*mCurrentBatch].reset();

    for(auto &observer : (*mObservers)[*mCurrentBatch])
        observer->notify();
    (*mTemporaryResources)[*mCurrentBatch].clear();
}
