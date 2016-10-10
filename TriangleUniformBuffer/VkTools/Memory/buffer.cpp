#include "buffer.hpp"
#include "chunkallocator.hpp"

void swap(Buffer &b1, Buffer &b2) {
    using std::swap;
    swap(static_cast<VkResource&>(b1), static_cast<VkResource&>(b2));
    swap(b1.mAllocator, b2.mAllocator);
    swap(b1.mBlock, b2.mBlock);
    swap(b1.mProperties, b2.mProperties);
    swap(b1.mPtr, b2.mPtr);
    swap(b1.mRequirements, b2.mRequirements);
    swap(b1.mSize, b2.mSize);
    swap(b1.mUsage, b2.mUsage);
    swap(b1.m_buffer , b2.m_buffer);
}

Buffer::Buffer(Buffer &&buffer) :
    VkResource(std::move(buffer)),
    vk::Buffer(buffer) {
    swap(*this, buffer);
}

Buffer::Buffer(const Buffer &buffer) :
    VkResource(buffer), vk::Buffer(buffer) {
    mAllocator = buffer.mAllocator;
    mBlock = buffer.mBlock;
    mProperties = buffer.mProperties;
    mPtr = buffer.mPtr;
    mRequirements = buffer.mRequirements;
    mSize = buffer.mSize;
    mUsage = buffer.mUsage;
}

Buffer &Buffer::operator=(Buffer buffer) {
    swap(*this, buffer);
    return *this;
}

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
    if(mCount != nullptr && --(*mCount) == 0) {
        mAllocator->deallocate(mBlock);
        mDevice.destroyBuffer(m_buffer);
    }
}
