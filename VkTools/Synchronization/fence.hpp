#pragma once
#include "vulkan/vkresource.hpp"

class Fence : public VkResource, public vk::Fence
{
public:
    Fence() = default;

    /**
     * @brief Fence
     * @param device
     * @param signaled : Is fence signaled at creation
     */
    Fence(Device const &device, bool signaled);

    Fence(Fence &&fence) = default;
    Fence(Fence const &fence) = default;
    Fence &operator=(Fence fence);

    void reset();
    void wait();

    ~Fence();
};
