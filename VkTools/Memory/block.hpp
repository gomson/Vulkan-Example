#pragma once
#include "vulkan/vkresource.hpp"

/**
 * @brief findMemoryType To know which memoryType we have to use with requirements.memoryTypeBits
 * @param memoryTypeBits
 * @param properties
 * @param shouldBeDeviceLocal : Is this memory be placed into the gpu device local memory or not?
 * @return
 */
int findMemoryType(uint32_t memoryTypeBits,
                   vk::PhysicalDeviceMemoryProperties const &properties,
                   bool shouldBeDeviceLocal);

struct Block {
    vk::DeviceMemory memory;
    vk::DeviceSize offset;
    vk::DeviceSize size;
    bool free = false;
    void *ptr = nullptr; // Useless if it is a GPU allocation

    bool operator ==(Block const &block);
};
