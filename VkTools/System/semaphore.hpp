#pragma once
#include "vulkan/vkresource.hpp"

class Semaphore : public VkResource, public vk::Semaphore
{
public:
    Semaphore(Device &device);

    Semaphore(Semaphore &&semaphore) = default;
    Semaphore(Semaphore const &semaphore) = default;
    Semaphore &operator=(Semaphore semaphore);

    ~Semaphore();
};
