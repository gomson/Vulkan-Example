#pragma once
#include "block.hpp"

/**
 * @brief The AbstractAllocator Let the user to allocate or deallocate some blocks
 */
class AbstractAllocator : private NotCopyable
{
public:
    AbstractAllocator(Device const &device) :
        mDevice(std::make_shared<Device>(device)) {

    }

    virtual Block allocate(vk::DeviceSize size, vk::DeviceSize alignment, int memoryTypeIndex) = 0;
    virtual void deallocate(Block &block) = 0;

    Device &getDevice() const {
        return *mDevice;
    }

    virtual ~AbstractAllocator() = 0;

protected:
    std::shared_ptr<Device> mDevice;
};

inline AbstractAllocator::~AbstractAllocator() {

}
