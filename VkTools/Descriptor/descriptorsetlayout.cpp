#include "descriptorsetlayout.hpp"

DescriptorSetLayout::DescriptorSetLayout(const Device &device, vk::DescriptorSetLayoutCreateInfo info) :
    VkResource(device) {
    m_descriptorSetLayout = device.createDescriptorSetLayout(info);
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyDescriptorSetLayout(m_descriptorSetLayout);
}
