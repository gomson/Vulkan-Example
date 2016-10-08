#pragma once
#include "vulkan/vkresource.hpp"

class Semaphore : public VkResource, public vk::Semaphore
{
public:
    Semaphore(vk::Device const &device);

    ~Semaphore();
};
