#include "pipelinelayout.hpp"

PipelineLayout::PipelineLayout(const Device &device) :
    VkResource(device),
    mDescriptorSetLayouts(std::make_shared<std::vector<vk::DescriptorSetLayout>>()),
    mDescriptorSets(std::make_shared<std::vector<vk::DescriptorSet>>()) {

}

std::vector<vk::DescriptorSetLayout>
PipelineLayout::getDescriptorSetLayouts() const {
    return *mDescriptorSetLayouts;
}

std::vector<vk::DescriptorSet>
PipelineLayout::getDescriptorSets() const {
    return *mDescriptorSets;
}

PipelineLayout::~PipelineLayout() {
    if(mCount != nullptr && --(*mCount) == 0) {
        for(auto &descriptorSetLayout : *mDescriptorSetLayouts)
            mDevice->destroyDescriptorSetLayout(descriptorSetLayout);
        mDevice->destroyPipelineLayout(*this);
    }
}
