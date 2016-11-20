#pragma once
#include "device.hpp"
#include "vulkan/vkresource.hpp"

class ShaderModule : public VkResource, public vk::ShaderModule
{
public:
    ShaderModule() = default;
    ShaderModule(Device const &device, std::string const &path);

    ShaderModule(ShaderModule &&shaderModule) = default;
    ShaderModule(ShaderModule const &shaderModule) = default;
    ShaderModule &operator=(ShaderModule shaderModule);

    ~ShaderModule();
};
