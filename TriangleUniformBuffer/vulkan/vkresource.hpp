#pragma once
#include "counter.hpp"
#include "VkTools/System/device.hpp"

class VkResource : public Counter
{
public:
    VkResource() = default;
    VkResource(Device device);
    VkResource(VkResource &&vkResource) = default;
    VkResource(VkResource const &vkResource) = default;
    VkResource &operator=(VkResource vkResource);

protected:
    Device mDevice;
};
