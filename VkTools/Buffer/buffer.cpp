#include "buffer.hpp"

Buffer::Buffer(const Device &device, vk::BufferUsageFlags usage, vk::DeviceSize size,
               std::shared_ptr<AbstractAllocator> allocator, bool shouldBeDeviceLocal) :
    VkResource(device), mAllocator(allocator),
    mSize(std::make_shared<vk::DeviceSize>(size)),
    mUsage(std::make_shared<vk::BufferUsageFlags>(usage)),
    mProperties(std::make_shared<vk::PhysicalDeviceMemoryProperties>(device.getPhysicalDevice().getMemoryProperties())),
    mIsDeviceLocal(std::make_shared<bool>(shouldBeDeviceLocal)) {
    createBuffer();
    allocate(shouldBeDeviceLocal);
}

Buffer &Buffer::operator =(Buffer buffer) {
    using std::swap;
    swap(static_cast<VkResource&>(buffer), static_cast<VkResource&>(*this));
    swap(static_cast<vk::Buffer&>(buffer), static_cast<vk::Buffer&>(*this));
    swap(buffer.mAllocator, mAllocator);
    swap(buffer.mBlock, mBlock);
    swap(buffer.mIsDeviceLocal, mIsDeviceLocal);
    swap(buffer.mProperties, mProperties);
    swap(buffer.mPtr, mPtr);
    swap(buffer.mRequirements, mRequirements);
    swap(buffer.mSize, mSize);
    swap(buffer.mUsage, mUsage);
    return *this;
}

vk::DeviceSize Buffer::getSize() const {
    return *mSize;
}

vk::BufferUsageFlags Buffer::getUsage() const {
    return *mUsage;
}

bool Buffer::isDeviceLocal() const {
    return *mIsDeviceLocal;
}

void *Buffer::getPtr() {
    return *mPtr;
}

std::shared_ptr<AbstractAllocator> Buffer::getAllocator() {
    return mAllocator;
}

void Buffer::createBuffer() {
    vk::BufferCreateInfo createInfo(vk::BufferCreateFlags(),
                                    *mSize,
                                    *mUsage,
                                    vk::SharingMode::eExclusive);

    m_buffer = mDevice->createBuffer(createInfo);
    *mRequirements = mDevice->getBufferMemoryRequirements(m_buffer);
}

void Buffer::allocate(bool shouldBeDeviceLocal) {
    int memoryTypeIndex = findMemoryType(mRequirements->memoryTypeBits, *mProperties, shouldBeDeviceLocal);

    *mBlock = mAllocator->allocate(mRequirements->size, mRequirements->alignment, memoryTypeIndex);
    mDevice->bindBufferMemory(m_buffer, mBlock->memory, mBlock->offset);

    // if host_visible, we can map it
    if(!shouldBeDeviceLocal)
        *mPtr = mBlock->ptr;
}

Buffer::~Buffer() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0) {
        mAllocator->deallocate(*mBlock);
        mDevice->destroyBuffer(m_buffer);
    }
}
