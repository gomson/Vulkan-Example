#pragma once
#include "vulkan/vkresource.hpp"

class Fence : public VkResource, public vk::Fence
{
public:
    /**
     * @brief Fence
     * @param device
     * @param signaled : Is fence signaled at creation
     */
    Fence(vk::Device &device, bool signaled);

    void reset();
    void wait();

    ~Fence();
};
