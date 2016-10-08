#pragma once
#include "instance.hpp"

class Device : public vk::Device, private NotCopyable
{
public:
    Device(Instance &instance);

    vk::PhysicalDevice getPhysicalDevice() const;

    uint32_t getIndexFamillyQueue() const;

    vk::Queue getQueue();

    ~Device();

private:
    vk::PhysicalDevice mPhysicalDevice;
    uint32_t mIndexFamillyQueue = 0;
};
