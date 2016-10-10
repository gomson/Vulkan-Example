#pragma once
#include "instance.hpp"

class Device : public vk::Device, private NotCopyable
{
public:
    Device(Instance &instance);

    vk::PhysicalDevice getPhysicalDevice() const;

    uint32_t getIndexFamillyQueue() const;
    uint32_t getIndexTransferQueue() const;
    uint32_t getIndexComputeQueue() const;

    vk::Queue getGraphicQueue();
    vk::Queue getComputeQueue();
    vk::Queue getTransferQueue();

    ~Device();

private:
    vk::PhysicalDevice mPhysicalDevice;
    uint32_t mIndexFamillyQueue = 0;
};
