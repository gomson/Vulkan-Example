#include "descriptorpool.hpp"

DescriptorPool::DescriptorPool(const Device &device, uint32_t number,
                               DescriptorSetLayout descriptorSetLayout) :
    VkResource(device),
    mDescriptorSetLayout(std::make_shared<DescriptorSetLayout>(descriptorSetLayout)),
    mDescriptorSets(std::make_shared<std::vector<vk::DescriptorSet>>(number)),
    mNumberDescriptorSetAllocated(std::make_shared<uint32_t>(0)) {
    std::vector<vk::DescriptorPoolSize> poolSizes;
    for(auto &types : descriptorSetLayout.getDescriptorTypes())
        poolSizes.emplace_back(types.first, types.second * number);

    vk::DescriptorPoolCreateInfo createInfo(
                vk::DescriptorPoolCreateFlags(),
                number, poolSizes.size(), poolSizes.data());

    m_descriptorPool = device.createDescriptorPool(createInfo);

    std::vector<vk::DescriptorSetLayout> layouts(number, descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocateInfo(*this, number, layouts.data());
    *mDescriptorSets = device.allocateDescriptorSets(allocateInfo);
}

DescriptorPool &DescriptorPool::operator=(DescriptorPool descriptorPool) {
    using std::swap;
    swap(static_cast<VkResource&>(descriptorPool), static_cast<VkResource&>(*this));
    swap(static_cast<vk::DescriptorPool&>(descriptorPool), static_cast<vk::DescriptorPool&>(*this));
    swap(descriptorPool.mDescriptorSetLayout, mDescriptorSetLayout);
    swap(descriptorPool.mDescriptorSets, mDescriptorSets);
    swap(descriptorPool.mNumberDescriptorSetAllocated, mNumberDescriptorSetAllocated);
    return *this;
}

vk::DescriptorSet DescriptorPool::allocate() {
    assert(*mNumberDescriptorSetAllocated != mDescriptorSets->size());
    return (*mDescriptorSets)[(*mNumberDescriptorSetAllocated)++];
}

bool DescriptorPool::isOneSetAvailable() const {
    return *mNumberDescriptorSetAllocated < mDescriptorSets->size();
}

void DescriptorPool::reset() {
    mDevice->resetDescriptorPool(m_descriptorPool);
}

DescriptorPool::~DescriptorPool() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyDescriptorPool(m_descriptorPool);
}
