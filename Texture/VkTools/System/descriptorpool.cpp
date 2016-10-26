#include "descriptorpool.hpp"

void swap(DescriptorPool &p1, DescriptorPool &p2) {
    using std::swap;
    swap(static_cast<VkResource&>(p1), static_cast<VkResource&>(p2));
    swap(p1.m_descriptorPool, p2.m_descriptorPool);
}

DescriptorPool::DescriptorPool(DescriptorPool &&descriptorPool) :
    VkResource(descriptorPool),
    vk::DescriptorPool(descriptorPool) {
    swap(*this, descriptorPool);
}

DescriptorPool::DescriptorPool(const DescriptorPool &descriptorPool) :
    VkResource(descriptorPool),
    vk::DescriptorPool(descriptorPool) {

}

DescriptorPool &DescriptorPool::operator=(DescriptorPool descriptorPool) {
    swap(*this, descriptorPool);
    return *this;
}

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
        mDevice.destroyDescriptorPool(m_descriptorPool);
}
