#pragma once
#include "vulkan/counter.hpp"
#include "instance.hpp"

class Device : public Counter, public vk::Device
{
public:
    Device() = default;
    Device(Device &&device) = default;
    Device(const Device &device) = default;
    Device &operator=(Device device);

    Device(Instance &instance);

    vk::PhysicalDevice getPhysicalDevice() const;

    uint32_t getIndexFamillyQueue() const;
    uint32_t getIndexTransferQueue() const;
    uint32_t getIndexComputeQueue() const;

    vk::Queue getGraphicQueue() const;
    vk::Queue getComputeQueue() const;
    vk::Queue getTransferQueue() const;

    ~Device();

private:
    std::shared_ptr<vk::PhysicalDevice> mPhysicalDevice;
    std::shared_ptr<uint32_t> mIndexFamillyQueue = std::make_shared<uint32_t>(0);
};
