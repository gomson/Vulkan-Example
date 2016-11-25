#include "sampler.hpp"

Sampler::Sampler(const Device &device, float maxMipMap) :
    VkResource(device) {
    vk::SamplerCreateInfo info(vk::SamplerCreateFlags(),
                               vk::Filter::eLinear,
                               vk::Filter::eLinear,
                               vk::SamplerMipmapMode::eLinear,
                               vk::SamplerAddressMode::eRepeat,
                               vk::SamplerAddressMode::eRepeat,
                               vk::SamplerAddressMode::eRepeat,
                               0, false, 0, false,
                               vk::CompareOp::eNever,
                               0, maxMipMap, vk::BorderColor::eFloatOpaqueWhite,
                               false);
    m_sampler = device.createSampler(info);
}

Sampler::Sampler(const Device &device, const vk::SamplerCreateInfo &info) :
    VkResource(device),
    vk::Sampler(device.createSampler(info)) {

}

Sampler::~Sampler() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroySampler(m_sampler);
}
