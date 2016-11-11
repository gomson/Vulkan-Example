#include "sampler.hpp"

void swap(Sampler &s1, Sampler &s2) {
    using std::swap;
    swap(static_cast<VkResource&>(s1), static_cast<VkResource&>(s2));
    swap(s1.m_sampler, s2.m_sampler);
}

Sampler::Sampler(Sampler &&sampler) :
    VkResource(sampler),
    vk::Sampler(sampler) {
    swap(*this, sampler);
}

Sampler::Sampler(Sampler const &sampler) :
    VkResource(sampler),
    vk::Sampler(sampler) {
    m_sampler = sampler.m_sampler;
}

Sampler &Sampler::operator =(Sampler sampler) {
    swap(*this, sampler);
    return *this;
}

Sampler::Sampler(Device &device, float maxMipMap) :
    VkResource(device) {
    vk::SamplerCreateInfo info(vk::SamplerCreateFlags(),
                               vk::Filter::eNearest,
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

Sampler::Sampler(Device &device, const vk::SamplerCreateInfo &info) :
    VkResource(device),
    vk::Sampler(device.createSampler(info)) {

}

Sampler::~Sampler() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice.destroySampler(m_sampler);
}
