#pragma once
#include <unordered_set>
#include "../System/device.hpp"
#include "../Command/commandpool.hpp"
#include "../Synchronization/fence.hpp"
#include "../Buffer/buffer.hpp"
#include "../Image/image.hpp"

class ObserverCommandBufferSubmitter {
public:
    virtual void notify() = 0;
};

class CommandBufferSubmitter
{
public:
    CommandBufferSubmitter(Device const &device, CommandPool &commandPool, uint32_t numberCommandBuffers);

    vk::CommandBuffer createCommandBuffer(ObserverCommandBufferSubmitter *observer);

    void cacheResource(std::shared_ptr<VkResource> &resource);

    void submit(uint32_t waitSemaphoreCount = 0, const vk::Semaphore *waitSemaphores = nullptr,
                vk::PipelineStageFlags *waitDstStageMask = nullptr,
                uint32_t signalSemaphoreCount = 0,
                const vk::Semaphore *signalSemaphores = nullptr);

    void setNumberBatch(uint32_t numberBatch);
    void setCurrentBatch(uint32_t i);

    void wait();

protected:
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<vk::Queue> mQueue;
    std::shared_ptr<CommandPool> mCommandPool;
    std::shared_ptr<uint32_t> mNumberCommandBufferToAllocate;
    std::shared_ptr<uint32_t> mCommandBufferIndex = std::make_shared<uint32_t>(0);
    std::shared_ptr<uint32_t> mCurrentBatch = std::make_shared<uint32_t>(0);

    std::shared_ptr<std::vector<std::vector<vk::CommandBuffer>>> mCommandBuffers = std::make_shared<std::vector<std::vector<vk::CommandBuffer>>>();
    std::shared_ptr<std::vector<Fence>> mFences = std::make_shared<std::vector<Fence>>();

    // If the buffer / image was only created for one transfer, it could be interesting to cache it
    std::shared_ptr<std::vector<std::vector<std::shared_ptr<VkResource>>>> mTemporaryResources = std::make_shared<std::vector<std::vector<std::shared_ptr<VkResource>>>>();

    std::shared_ptr<std::vector<std::unordered_set<ObserverCommandBufferSubmitter*>>> mObservers = std::make_shared<std::vector<std::unordered_set<ObserverCommandBufferSubmitter*>>>();
};
