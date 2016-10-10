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
    Fence(vk::Device &device, bool signaled);

    friend void swap(Fence &f1, Fence &f2);
    Fence(Fence &&fence);
    Fence(Fence const &fence);
    Fence &operator=(Fence fence);

    void reset();
    void wait();

    ~Fence();
};
