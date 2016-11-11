#pragma once
#include "block.hpp"

/**
 * @brief The AbstractAllocator Let the user to allocate or deallocate some blocks
 */
class AbstractAllocator : private NotCopyable
{
public:
    AbstractAllocator(Device const &device) :
        mDevice(device) {

    }

    virtual Block allocate(vk::DeviceSize size, int memoryTypeIndex) = 0;
    virtual void deallocate(Block &block) = 0;

    Device getDevice() const {
        return mDevice;
    }

    virtual ~AbstractAllocator() = 0;

protected:
    Device mDevice;
};

inline AbstractAllocator::~AbstractAllocator() {

}
