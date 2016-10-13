#pragma once
#include "vulkan/vkresource.hpp"

class Semaphore : public VkResource, public vk::Semaphore
{
public:
    Semaphore(Device &device);

    friend void swap(Semaphore &s1, Semaphore &s2);
    Semaphore(Semaphore &&semaphore);
    Semaphore(Semaphore const &semaphore);
    Semaphore &operator=(Semaphore semaphore);

    ~Semaphore();
};
