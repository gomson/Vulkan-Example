#pragma once
#include "VkTools/Descriptor/descriptorsetlayout.hpp"

class DescriptorPool : public VkResource, public vk::DescriptorPool
{
public:
    DescriptorPool() = default;
    DescriptorPool(Device const &device, uint32_t number,
                   DescriptorSetLayout descriptorSetLayout);

    DescriptorPool(DescriptorPool &&descriptorPool) = default;
    DescriptorPool(DescriptorPool const &descriptorPool) = default;
    DescriptorPool &operator=(DescriptorPool descriptorPool);

    vk::DescriptorSet allocate();
    bool isOneSetAvailable() const;
    void reset();

    ~DescriptorPool();
private:
    std::shared_ptr<DescriptorSetLayout> mDescriptorSetLayout;
    std::shared_ptr<std::vector<vk::DescriptorSet>> mDescriptorSets;
    std::shared_ptr<uint32_t> mNumberDescriptorSetAllocated;
};
