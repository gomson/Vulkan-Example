#include "materialdescriptorsetpool.hpp"

static std::vector<vk::DescriptorPoolSize> materialPoolSize(uint32_t number) {
    return std::vector<vk::DescriptorPoolSize>{
        vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, number)
    };
}

MaterialDescriptorSetPool::MaterialDescriptorSetPool(Device &device, uint32_t number, std::shared_ptr<DescriptorSetLayout> layout) :
    DescriptorPool(device, number, materialPoolSize(number)),
    mDescriptorSetLayout(layout),
    mDescriptorSets(std::make_shared<std::vector<vk::DescriptorSet>>(number)),
    mNumberDescriptorSetAllocated(std::make_shared<uint32_t>(0)) {

    std::vector<vk::DescriptorSetLayout> layouts(number, *mDescriptorSetLayout);
    vk::DescriptorSetAllocateInfo info(*this, number, layouts.data());

    *mDescriptorSets = device.allocateDescriptorSets(info);
}

vk::DescriptorSet MaterialDescriptorSetPool::allocate() {
    assert(*mNumberDescriptorSetAllocated != mDescriptorSets->size());
    return (*mDescriptorSets)[(*mNumberDescriptorSetAllocated)++];
}


MaterialDescriptorSetManager::MaterialDescriptorSetManager(Device &device) :
    mDevice(std::make_shared<Device>(device)),
    mPools(std::make_shared<std::vector<MaterialDescriptorSetPool>>()){
    std::vector<vk::DescriptorSetLayoutBinding> bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr)
    };

    vk::DescriptorSetLayoutCreateInfo layoutInfo(vk::DescriptorSetLayoutCreateFlags(), bindings.size(),
                                                 bindings.data());

    mDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(device, layoutInfo);
}

vk::DescriptorSet MaterialDescriptorSetManager::allocate() {
    for(auto &pool : *mPools)
        if(pool.getNumberDescriptorSetAllocated() != pool.getMaximumNumberDescriptorSet())
            return pool.allocate();

    mPools->emplace_back(*mDevice, 16, mDescriptorSetLayout);
    return allocate();
}
