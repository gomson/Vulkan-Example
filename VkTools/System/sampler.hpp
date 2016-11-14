#pragma once
#include "vulkan/vkresource.hpp"

class Sampler : public VkResource, public vk::Sampler
{
public:
    Sampler(Device &device, float maxMipMap);
    Sampler(Device &device, vk::SamplerCreateInfo const &info);
    Sampler(Sampler &&sampler) = default;
    Sampler(Sampler const &sampler) = default;
    Sampler &operator=(Sampler sampler);

    ~Sampler();

private:
};
