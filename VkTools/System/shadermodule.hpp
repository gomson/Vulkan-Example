#pragma once
#include "device.hpp"
#include "vulkan/vkresource.hpp"

class ShaderModule : public VkResource, public vk::ShaderModule
{
public:
    ShaderModule(Device &device, std::string const &path);

    ShaderModule(ShaderModule &&shaderModule) = default;
    ShaderModule(ShaderModule const &shaderModule) = default;
    ShaderModule &operator=(ShaderModule shaderModule);

    ~ShaderModule();
};
