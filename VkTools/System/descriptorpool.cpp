#include "descriptorpool.hpp"

DescriptorPool::DescriptorPool(Device &device, uint32_t maxSet,
                               const vk::ArrayProxy<vk::DescriptorPoolSize> &poolSizes) :
    VkResource(device)
{
    vk::DescriptorPoolCreateInfo createInfo(
                vk::DescriptorPoolCreateFlags(),
                maxSet, poolSizes.size(), poolSizes.data());

    m_descriptorPool = device.createDescriptorPool(createInfo);
}

DescriptorPool::~DescriptorPool() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyDescriptorPool(m_descriptorPool);
}
