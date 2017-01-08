#pragma once
#include "vulkan/vkresource.hpp"

class Sampler : public VkResource, public vk::Sampler
{
public:
    Sampler() = default;
    Sampler(Device const &device, float maxMipMap);
    Sampler(Device const &device, vk::SamplerCreateInfo const &info);
    Sampler(Sampler &&sampler) = default;
    Sampler(Sampler const &sampler) = default;
    Sampler &operator=(Sampler sampler);

    ~Sampler();

private:
};
