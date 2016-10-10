#pragma once
#include "VkTools/System/device.hpp"
#include "abstractallocator.hpp"

class Buffer : public VkResource, public vk::Buffer
{
public:
    Buffer() = default;

    // On non device local
    Buffer(Device &device, vk::BufferUsageFlags usage, vk::DeviceSize size,
           void *data,
           std::shared_ptr<AbstractAllocator> allocator);

    // On non device local
    template<typename T>
    Buffer(Device &device, vk::BufferUsageFlags usage, vk::ArrayProxy<T> const &data,
           std::shared_ptr<AbstractAllocator> allocator);

    // On device local
    Buffer(Device &device, vk::BufferUsageFlags usage, vk::DeviceSize size,
           std::shared_ptr<AbstractAllocator> allocator);

    Buffer(Buffer &&buffer);
    Buffer(Buffer const &buffer);
    Buffer &operator=(Buffer buffer);
    friend void swap(Buffer &b1, Buffer &b2);

    vk::DeviceSize getSize() const;
    vk::BufferUsageFlags getUsage() const;

    ~Buffer();

private:
    std::shared_ptr<AbstractAllocator> mAllocator;
    vk::DeviceSize mSize;
    vk::BufferUsageFlags mUsage;
    vk::MemoryRequirements mRequirements;
    vk::PhysicalDeviceMemoryProperties mProperties;
    Block mBlock;
    void *mPtr;

    void createBuffer();
    void allocate(bool shouldBeDeviceLocal);
};

// Host visible memory
template<typename T>
Buffer::Buffer(Device &device, vk::BufferUsageFlags usage,
               vk::ArrayProxy<T> const &data,
               std::shared_ptr<AbstractAllocator> allocator) :
    VkResource(device), mAllocator(allocator), mSize(sizeof(T) * data.size()),
    mUsage(usage),
    mProperties(device.getPhysicalDevice().getMemoryProperties()) {
    createBuffer();
    allocate(false);
    memcpy(mPtr, data.data(), mSize);
}
