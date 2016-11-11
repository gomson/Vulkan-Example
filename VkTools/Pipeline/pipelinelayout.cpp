#include "pipelinelayout.hpp"

void swap(PipelineLayout &p1, PipelineLayout &p2) {
    using std::swap;
    swap(static_cast<VkResource&>(p1), static_cast<VkResource&>(p2));
    swap(p1.mDescriptorSets, p2.mDescriptorSets);
    swap(p1.mDescriptorSetLayouts, p2.mDescriptorSetLayouts);
    swap(p1.m_pipelineLayout, p2.m_pipelineLayout);
}

PipelineLayout::PipelineLayout(PipelineLayout &&pipelineLayout) :
    VkResource(pipelineLayout),
    vk::PipelineLayout(pipelineLayout) {
    swap(*this, pipelineLayout);
}

PipelineLayout::PipelineLayout(const PipelineLayout &pipelineLayout) :
    VkResource(std::move(pipelineLayout)),
    vk::PipelineLayout(pipelineLayout) {
    mDescriptorSets = pipelineLayout.mDescriptorSets;
    mDescriptorSetLayouts = pipelineLayout.mDescriptorSetLayouts;
}

PipelineLayout &PipelineLayout::operator =(PipelineLayout pipelineLayout) {
    swap(*this, pipelineLayout);
    return *this;
}

PipelineLayout::PipelineLayout(Device &device) :
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
            mDevice.destroyDescriptorSetLayout(descriptorSetLayout);
        mDevice.destroyPipelineLayout(*this);
    }
}
