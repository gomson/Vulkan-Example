#pragma once
#include <deque>
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
    CommandBufferSubmitter(Device &device, CommandPool &commandPool, uint32_t numberCommandBuffers);

    vk::CommandBuffer createCommandBuffer(ObserverCommandBufferSubmitter *observer);

    void cacheResource(std::shared_ptr<VkResource> &resource);

    void submit();
    void submit(bool wait);
    void wait(uint32_t i);
    void waitAll();

protected:
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<vk::Queue> mQueue;
    std::shared_ptr<CommandPool> mCommandPool;
    std::shared_ptr<std::deque<std::vector<vk::CommandBuffer>>> mCommandBuffers = std::make_shared<std::deque<std::vector<vk::CommandBuffer>>>();
    std::shared_ptr<std::deque<Fence>> mFences = std::make_shared<std::deque<Fence>>();
    std::shared_ptr<uint32_t> mNumberCommandBufferToAllocate;
    std::shared_ptr<uint32_t> mCommandBufferIndex = std::make_shared<uint32_t>(0);
    std::shared_ptr<uint32_t> mSubmitIndex = std::make_shared<uint32_t>(0);

    // If the buffer / image was only created for one transfer, it could be interesting to cache it
    std::shared_ptr<std::deque<std::vector<std::shared_ptr<VkResource>>>> mTemporaryResources = std::make_shared<std::deque<std::vector<std::shared_ptr<VkResource>>>>();

    std::shared_ptr<std::deque<std::unordered_set<ObserverCommandBufferSubmitter*>>> mObservers = std::make_shared<std::deque<std::unordered_set<ObserverCommandBufferSubmitter*>>>();

    void addSubmitIndex();
    void destroySubmit(uint32_t i);
};
