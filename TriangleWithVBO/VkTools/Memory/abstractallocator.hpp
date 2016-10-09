#pragma once
#include "block.hpp"

/**
 * @brief The AbstractAllocator Let the user to allocate or deallocate some blocks
 */
class AbstractAllocator
{
public:
    virtual Block allocate(vk::DeviceSize size, int memoryTypeIndex) = 0;

    virtual void deallocate(Block &block) = 0;

    virtual ~AbstractAllocator() = 0;
};

inline AbstractAllocator::~AbstractAllocator() {

}
