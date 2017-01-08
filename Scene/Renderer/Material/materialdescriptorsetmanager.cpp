#include "materialdescriptorsetmanager.hpp"

#define SET_BY_POOL 32
#define MATERIAL_BY_BUFFER 32

static std::vector<vk::DescriptorType> materialDescriptorTypes() {
    return std::vector<vk::DescriptorType>{
        vk::DescriptorType::eCombinedImageSampler,
        vk::DescriptorType::eUniformBuffer
    };
}

MaterialDescriptorSetManager::MaterialDescriptorSetManager(Device &device, Transferer &transferer) :
    mDevice(std::make_shared<Device>(device)),
    mTransferer(std::make_shared<Transferer>(transferer)),
    mPools(std::make_shared<std::vector<DescriptorPool>>()),
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
        if(pool.isOneSetAvailable())
            return pool.allocate();

    mPools->emplace_back(*mDevice, SET_BY_POOL, materialDescriptorTypes(), *mDescriptorSetLayout);
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
