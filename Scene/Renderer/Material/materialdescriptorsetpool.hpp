#pragma once
#include "VkTools/Buffer/buffer.hpp"
#include "VkTools/Descriptor/descriptorpool.hpp"
#include "VkTools/Descriptor/descriptorsetlayout.hpp"
#include "VkTools/Command/transferer.hpp"

struct alignas(256) MaterialUniform {
    glm::vec4 color;
    int hasDiffuseTexture;
};

static_assert(sizeof(MaterialUniform) % 256 == 0, "MaterialUniform should be 256 bytes aligned");

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
    MaterialDescriptorSetManager(Device &device, Transferer &transferer);
    vk::DescriptorSet allocate();
    // pair Buffer offset
    std::pair<vk::Buffer, uint32_t> addMaterialToBuffer(MaterialUniform material);
    DescriptorSetLayout getDescriptorSetLayout() {return *mDescriptorSetLayout;}

private:
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<Transferer> mTransferer;
    std::shared_ptr<DescriptorSetLayout> mDescriptorSetLayout;
    std::shared_ptr<std::vector<MaterialDescriptorSetPool>> mPools;
    std::shared_ptr<std::vector<std::pair<uint32_t, Buffer>>> mMaterialUniformBuffers;
};
