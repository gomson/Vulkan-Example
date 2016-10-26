#pragma once
#include "VkTools/System/device.hpp"
#include "abstractallocator.hpp"

class Buffer : public VkResource, public vk::Buffer
{
public:
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
    std::shared_ptr<vk::DeviceSize> mSize = std::make_shared<vk::DeviceSize>();
    std::shared_ptr<vk::BufferUsageFlags> mUsage = std::make_shared<vk::BufferUsageFlags>();
    std::shared_ptr<vk::MemoryRequirements> mRequirements = std::make_shared<vk::MemoryRequirements>();
    std::shared_ptr<vk::PhysicalDeviceMemoryProperties> mProperties = std::make_shared<vk::PhysicalDeviceMemoryProperties>();
    std::shared_ptr<Block> mBlock = std::make_shared<Block>();
    std::shared_ptr<void *> mPtr = std::make_shared<void *>(nullptr);

    void createBuffer();
    void allocate(bool shouldBeDeviceLocal);
};

// Host visible memory
template<typename T>
Buffer::Buffer(Device &device, vk::BufferUsageFlags usage,
               vk::ArrayProxy<T> const &data,
               std::shared_ptr<AbstractAllocator> allocator) :
    VkResource(device), mAllocator(allocator),
    mSize(std::make_shared<vk::DeviceSize>(sizeof(T) * data.size())),
    mUsage(std::make_shared<vk::BufferUsageFlags>(usage)),
    mProperties(std::make_shared<vk::PhysicalDeviceMemoryProperties>(device.getPhysicalDevice().getMemoryProperties())) {
    createBuffer();
    allocate(false);
    memcpy(*mPtr, data.data(), *mSize);
}
