#pragma once
#include "VkTools/Descriptor/descriptorpool.hpp"
#include "VkTools/Descriptor/descriptorsetlayout.hpp"

class MaterialDescriptorSetPool : public DescriptorPool
{
public:
    MaterialDescriptorSetPool(Device &device, uint32_t number, std::shared_ptr<DescriptorSetLayout> layout);

    uint32_t getNumberDescriptorSetAllocated() const {return *mNumberDescriptorSetAllocated;}
    uint32_t getMaximumNumberDescriptorSet() const {return mDescriptorSets->size();}
    vk::DescriptorSet allocate();

private:
    std::shared_ptr<DescriptorSetLayout> mDescriptorSetLayout;
    std::shared_ptr<std::vector<vk::DescriptorSet>> mDescriptorSets;
    std::shared_ptr<uint32_t> mNumberDescriptorSetAllocated;
};

class MaterialDescriptorSetManager {
public:
    MaterialDescriptorSetManager(Device &device);
    vk::DescriptorSet allocate();
private:
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<DescriptorSetLayout> mDescriptorSetLayout;
    std::shared_ptr<std::vector<MaterialDescriptorSetPool>> mPools;
};
