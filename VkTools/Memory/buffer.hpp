#pragma once
#include "VkTools/System/device.hpp"
#include "abstractallocator.hpp"

class Buffer : public VkResource, public vk::Buffer
{
public:
    Buffer() = default;

    Buffer(Device const &device, vk::BufferUsageFlags usage, vk::DeviceSize size,
           std::shared_ptr<AbstractAllocator> allocator, bool shouldBeDeviceLocal);

    Buffer(Buffer &&buffer) = default;
    Buffer(Buffer const &buffer) = default;
    Buffer &operator=(Buffer buffer);

    vk::DeviceSize getSize() const;
    vk::BufferUsageFlags getUsage() const;
    bool isDeviceLocal() const;
    void *getPtr();
    std::shared_ptr<AbstractAllocator> getAllocator();

    ~Buffer();

private:
    std::shared_ptr<AbstractAllocator> mAllocator;
    std::shared_ptr<vk::DeviceSize> mSize = std::make_shared<vk::DeviceSize>();
    std::shared_ptr<vk::BufferUsageFlags> mUsage = std::make_shared<vk::BufferUsageFlags>();
    std::shared_ptr<vk::MemoryRequirements> mRequirements = std::make_shared<vk::MemoryRequirements>();
    std::shared_ptr<vk::PhysicalDeviceMemoryProperties> mProperties = std::make_shared<vk::PhysicalDeviceMemoryProperties>();
    std::shared_ptr<Block> mBlock = std::make_shared<Block>();
    std::shared_ptr<bool> mIsDeviceLocal;
    std::shared_ptr<void *> mPtr = std::make_shared<void *>(nullptr);

    void createBuffer();
    void allocate(bool shouldBeDeviceLocal);
};
