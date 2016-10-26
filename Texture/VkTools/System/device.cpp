#include <cassert>
#include "device.hpp"
#include <iostream>

void swap(Device &d1, Device &d2) {
    using std::swap;
    swap(static_cast<Counter&>(d1), static_cast<Counter&>(d2));
    swap(d1.mIndexFamillyQueue, d2.mIndexFamillyQueue);
    swap(d1.mPhysicalDevice, d2.mPhysicalDevice);
    swap(d1.m_device, d2.m_device);
}

Device::Device(Device &&device) :
    Counter(device),
    vk::Device(device) {
    swap(*this, device);
}

Device::Device(Device const &device) :
    Counter(device),
    vk::Device(device) {
    mIndexFamillyQueue = device.mIndexFamillyQueue;
    mPhysicalDevice = device.mPhysicalDevice;
}

Device &Device::operator=(Device device) {
    swap(*this, device);
    return *this;
}

Device::Device(Instance &instance) :
    mPhysicalDevice(std::make_shared<vk::PhysicalDevice>(instance.enumeratePhysicalDevices()[0])) {
    std::vector<vk::QueueFamilyProperties> queueProperties = mPhysicalDevice->getQueueFamilyProperties();

    auto flags = vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer;

    // Look for the good queue
    for(vk::QueueFamilyProperties qp : queueProperties) {
        if((qp.queueFlags & flags) == flags)
            break;
        ++(*mIndexFamillyQueue);
    }

    assert(mPhysicalDevice->getSurfaceSupportKHR(*mIndexFamillyQueue, instance.getSurfaceKHR()));

    float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo(vk::DeviceQueueCreateFlags(),
                                              0, 1, &queuePriority);

    std::vector<char const *> extensions{{VK_KHR_SWAPCHAIN_EXTENSION_NAME}};
    vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(),
                                          1, &queueCreateInfo, 0, nullptr,
                                          1, &extensions[0], nullptr);

    m_device = mPhysicalDevice->createDevice(deviceCreateInfo);
}

vk::Queue Device::getGraphicQueue() {
    return getQueue(*mIndexFamillyQueue, 0);
}

vk::Queue Device::getTransferQueue() {
    return getQueue(*mIndexFamillyQueue, 0);
}

vk::Queue Device::getComputeQueue() {
    return getQueue(*mIndexFamillyQueue, 0);
}

vk::PhysicalDevice Device::getPhysicalDevice() const {
    return *mPhysicalDevice;
}

uint32_t Device::getIndexFamillyQueue() const {
    return *mIndexFamillyQueue;
}

uint32_t Device::getIndexTransferQueue() const {
    return *mIndexFamillyQueue;
}

uint32_t Device::getIndexComputeQueue() const {
    return *mIndexFamillyQueue;
}

Device::~Device() {
    if(mCount != nullptr && --(*mCount) == 0)
        destroy();
}
