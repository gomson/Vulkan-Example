#include "descriptorsetlayout.hpp"

DescriptorSetLayout::DescriptorSetLayout(const Device &device, vk::DescriptorSetLayoutCreateInfo info) :
    VkResource(device),
    mDescriptorTypes(std::make_shared<std::unordered_map<vk::DescriptorType, unsigned, DescriptorTypeHash>>())
{
    m_descriptorSetLayout = device.createDescriptorSetLayout(info);

    for(auto i(0u); i < info.bindingCount; ++i)
        (*mDescriptorTypes)[info.pBindings[i].descriptorType] += info.pBindings[i].descriptorCount;
}

DescriptorSetLayout &DescriptorSetLayout::operator =(DescriptorSetLayout descriptorSetLayout) {
    using std::swap;
    swap(static_cast<VkResource&>(descriptorSetLayout), static_cast<VkResource&>(*this));
    swap(static_cast<vk::DescriptorSetLayout&>(descriptorSetLayout), static_cast<vk::DescriptorSetLayout&>(*this));
    swap(descriptorSetLayout.mDescriptorTypes, mDescriptorTypes);
    return *this;
}


std::unordered_map<vk::DescriptorType, unsigned, DescriptorTypeHash> DescriptorSetLayout::getDescriptorTypes() const {
    return *mDescriptorTypes;
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyDescriptorSetLayout(m_descriptorSetLayout);
}
