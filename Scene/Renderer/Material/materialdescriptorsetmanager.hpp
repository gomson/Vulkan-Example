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
    std::shared_ptr<std::vector<DescriptorPool>> mPools;
    std::shared_ptr<std::vector<std::pair<uint32_t, Buffer>>> mMaterialUniformBuffers;
};
