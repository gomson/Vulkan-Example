#include "block.hpp"

int findMemoryType(uint32_t memoryTypeBits,
                   vk::PhysicalDeviceMemoryProperties const &properties,
                   bool shouldBeDeviceLocal) {

    auto lambdaGetMemoryType = [&](vk::MemoryPropertyFlags propertyFlags) -> int {
        for(uint32_t i = 0; i < properties.memoryTypeCount; ++i)
            if((memoryTypeBits & (1 << i)) &&
            ((properties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags))
                return i;
        return -1;
    };

    if(!shouldBeDeviceLocal) {
        vk::MemoryPropertyFlags optimal = vk::MemoryPropertyFlagBits::eHostCached |
                vk::MemoryPropertyFlagBits::eHostCoherent |
                vk::MemoryPropertyFlagBits::eHostVisible;

        vk::MemoryPropertyFlags required = vk::MemoryPropertyFlagBits::eHostCoherent |
                vk::MemoryPropertyFlagBits::eHostVisible;

        int type = lambdaGetMemoryType(optimal);
        if(type == -1)
            return lambdaGetMemoryType(required);
        return type;
    }

    else
        return lambdaGetMemoryType(vk::MemoryPropertyFlagBits::eDeviceLocal);
}

bool Block::operator ==(Block const &block) {
    if(memory == block.memory &&
       offset == block.offset &&
       size == block.size &&
       free == block.free)
        return true;
    return false;
}
