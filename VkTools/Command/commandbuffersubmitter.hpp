#pragma once
#include <deque>
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
    CommandBufferSubmitter(Device &device, uint32_t numberCommandBuffers);

    void addObserver(ObserverCommandBufferSubmitter *observer);

    vk::CommandBuffer createCommandBuffer();

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
    std::shared_ptr<uint32_t> mCommandBufferIndex = std::make_shared<uint32_t>(0);
    std::shared_ptr<uint32_t> mSubmitIndex = std::make_shared<uint32_t>(0);
    std::shared_ptr<std::vector<ObserverCommandBufferSubmitter*>> mObservers = std::make_shared<std::vector<ObserverCommandBufferSubmitter*>>();

    // If the buffer / image was only created for one transfer, it could be interesting to cache it
};
