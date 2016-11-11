#pragma once
#include "vulkan/counter.hpp"
#include "instance.hpp"

class Device : public Counter, public vk::Device
{
public:
    friend void swap(Device &d1, Device &d2);
    Device() = default;
    Device(Device &&device);
    Device(const Device &device);
    Device &operator=(Device device);

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
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<uint32_t> mIndexFamillyQueue = std::make_shared<uint32_t>(0);
};
