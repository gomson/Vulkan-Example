#pragma once
#include "vulkan/vkresource.hpp"

class Sampler : public VkResource, public vk::Sampler
{
public:
    friend void swap(Sampler &s1, Sampler &s2);
    Sampler(Device &device);
    Sampler(Device &device, vk::SamplerCreateInfo const &info);
    Sampler(Sampler &&sampler);
    Sampler(Sampler const &sampler);
    Sampler &operator=(Sampler sampler);

    ~Sampler();

private:
};
