#include "descriptorpool.hpp"

DescriptorPool::DescriptorPool(const Device &device, uint32_t maxSet,
                               std::vector<vk::DescriptorPoolSize> poolSizes) :
    VkResource(device)
{
    vk::DescriptorPoolCreateInfo createInfo(
                vk::DescriptorPoolCreateFlags(),
                maxSet, poolSizes.size(), poolSizes.data());

    m_descriptorPool = device.createDescriptorPool(createInfo);
}

DescriptorPool &DescriptorPool::operator=(DescriptorPool descriptorPool) {
    using std::swap;
    swap(static_cast<VkResource&>(descriptorPool), static_cast<VkResource&>(*this));
    swap(static_cast<vk::DescriptorPool&>(descriptorPool), static_cast<vk::DescriptorPool&>(*this));
    return *this;
}

DescriptorPool::~DescriptorPool() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyDescriptorPool(m_descriptorPool);
}
