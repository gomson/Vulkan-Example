#pragma once
#include "../System/device.hpp"
#include "../System/commandpool.hpp"
#include "../System/fence.hpp"

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
    void wait();

protected:
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<vk::Queue> mQueue;
    std::shared_ptr<CommandPool> mCommandPool;
    std::shared_ptr<std::vector<vk::CommandBuffer>> mCommandBuffers = std::make_shared<std::vector<vk::CommandBuffer>>();
    std::shared_ptr<Fence> mFence;
    std::shared_ptr<uint32_t> mIndex = std::make_shared<uint32_t>(0);
    std::shared_ptr<std::vector<ObserverCommandBufferSubmitter*>> mObservers = std::make_shared<std::vector<ObserverCommandBufferSubmitter*>>();
};
