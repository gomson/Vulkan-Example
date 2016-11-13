#pragma once
#include "counter.hpp"
#include "VkTools/System/device.hpp"

class VkResource : public Counter
{
public:
    VkResource() = default;
    VkResource(Device const &device);
    VkResource(VkResource &&vkResource) = default;
    VkResource(VkResource const &vkResource) = default;
    VkResource &operator=(VkResource const &vkResource) = default;

    vk::Device getDevice() const;

protected:
    std::shared_ptr<Device> mDevice;
};
