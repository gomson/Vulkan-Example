#include "materialdescriptorsetpool.hpp"

#define SET_BY_POOL 32
#define MATERIAL_BY_BUFFER 32

static std::vector<vk::DescriptorPoolSize> materialPoolSize(uint32_t number) {
    return std::vector<vk::DescriptorPoolSize>{
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, number),
        vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, number),
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


MaterialDescriptorSetManager::MaterialDescriptorSetManager(Device &device, Transferer &transferer) :
    mDevice(std::make_shared<Device>(device)),
    mTransferer(std::make_shared<Transferer>(transferer)),
    mPools(std::make_shared<std::vector<MaterialDescriptorSetPool>>()),
    mMaterialUniformBuffers(std::make_shared<std::vector<std::pair<uint32_t, Buffer>>>()){
    std::vector<vk::DescriptorSetLayoutBinding> bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment, nullptr),
        vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr),
    };

    vk::DescriptorSetLayoutCreateInfo layoutInfo(vk::DescriptorSetLayoutCreateFlags(), bindings.size(),
                                                 bindings.data());

    mDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(device, layoutInfo);
}

vk::DescriptorSet MaterialDescriptorSetManager::allocate() {
    for(auto &pool : *mPools)
        if(pool.getNumberDescriptorSetAllocated() != pool.getMaximumNumberDescriptorSet())
            return pool.allocate();

    mPools->emplace_back(*mDevice, SET_BY_POOL, mDescriptorSetLayout);
    return allocate();
}

std::pair<vk::Buffer, uint32_t> MaterialDescriptorSetManager::addMaterialToBuffer(MaterialUniform material) {
    for(auto &buffer : *mMaterialUniformBuffers) {
        if(buffer.first < MATERIAL_BY_BUFFER) {
            uint32_t offset = buffer.first++ * sizeof(MaterialUniform);
            mTransferer->transfer(buffer.second, offset, sizeof(MaterialUniform), &material, vk::PipelineStageFlagBits::eFragmentShader);
            return std::make_pair(buffer.second, offset);
        }
    }

    mMaterialUniformBuffers->emplace_back(std::make_pair(0, Buffer(*mDevice, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer, MATERIAL_BY_BUFFER * sizeof(MaterialUniform), mTransferer->getAllocator(), true)));
    return addMaterialToBuffer(material);
}
