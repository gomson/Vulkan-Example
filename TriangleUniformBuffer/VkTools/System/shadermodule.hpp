#pragma once
#include "device.hpp"
#include "vulkan/vkresource.hpp"

class ShaderModule : public VkResource, public vk::ShaderModule
{
public:
    ShaderModule(Device &device, std::string const &path);

    friend void swap(ShaderModule &s1, ShaderModule &s2);
    ShaderModule(ShaderModule &&shaderModule);
    ShaderModule(ShaderModule const &shaderModule);
    ShaderModule &operator=(ShaderModule shaderModule);

    ~ShaderModule();
};
