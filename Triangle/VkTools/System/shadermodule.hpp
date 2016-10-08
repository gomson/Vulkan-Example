#pragma once
#include "device.hpp"
#include "vulkan/vkresource.hpp"

class ShaderModule : public vk::ShaderModule, public VkResource
{
public:
    ShaderModule(vk::Device &device, std::string const &path);

    ~ShaderModule();
};
