#include "buffer.hpp"
#include "chunkallocator.hpp"

// Host visible memory
Buffer::Buffer(Device &device, vk::BufferUsageFlags usage,
               vk::DeviceSize size, void *data,
               std::shared_ptr<AbstractAllocator> allocator) :
    VkResource(device), mAllocator(allocator), mSize(size), mUsage(usage),
    mProperties(device.getPhysicalDevice().getMemoryProperties()) {
    createBuffer();
    allocate(false); // allocate on the host_visible
    if(data != nullptr)
        memcpy(mPtr, data, size);
}

// Device local memory
Buffer::Buffer(Device &device, vk::BufferUsageFlags usage, vk::DeviceSize size,
               std::shared_ptr<AbstractAllocator> allocator) :
    VkResource(device), mAllocator(allocator), mSize(size), mUsage(usage),
    mProperties(device.getPhysicalDevice().getMemoryProperties()) {
    createBuffer();
    allocate(true); // allocate on the local_device
}

vk::DeviceSize Buffer::getSize() const {
    return mSize;
}

vk::BufferUsageFlags Buffer::getUsage() const {
    return mUsage;
}

void Buffer::createBuffer() {
    vk::BufferCreateInfo createInfo(vk::BufferCreateFlags(),
                                    mSize,
                                    mUsage,
                                    vk::SharingMode::eExclusive);

    m_buffer = mDevice.createBuffer(createInfo);
    mRequirements = mDevice.getBufferMemoryRequirements(m_buffer);
}

void Buffer::allocate(bool shouldBeDeviceLocal) {
    int memoryTypeIndex = findMemoryType(mRequirements.memoryTypeBits, mProperties, shouldBeDeviceLocal);

    mBlock = mAllocator->allocate(mRequirements.size, memoryTypeIndex);
    mDevice.bindBufferMemory(m_buffer, mBlock.memory, mBlock.offset);

    // if host_visible, we can map it
    if(!shouldBeDeviceLocal)
        mPtr = mDevice.mapMemory(mBlock.memory, mBlock.offset,
                                 mSize, vk::MemoryMapFlags());
}

Buffer::~Buffer() {
    mAllocator->deallocate(mBlock);
    mDevice.destroyBuffer(m_buffer);
}
