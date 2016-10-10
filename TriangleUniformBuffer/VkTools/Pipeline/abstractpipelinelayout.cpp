#include "abstractpipelinelayout.hpp"

AbstractPipelineLayout::AbstractPipelineLayout(Device &device) :
    VkResource(device)
{

}

std::vector<vk::DescriptorSetLayout> const&
AbstractPipelineLayout::getDescriptorSetLayouts() const {
    return mDescriptorSetLayouts;
}

std::vector<vk::DescriptorSet> const&
AbstractPipelineLayout::getDescriptorSet() const {
    return mDescriptorSet;
}

AbstractPipelineLayout::~AbstractPipelineLayout() {
    for(auto &descriptorSetLayout : mDescriptorSetLayouts)
        mDevice.destroyDescriptorSetLayout(descriptorSetLayout);
    mDevice.destroyPipelineLayout(*this);
}
