#include "pipelinelayout.hpp"

PipelineLayout::PipelineLayout(const Device &device) :
    VkResource(device) {

}

std::vector<DescriptorSetLayout>
PipelineLayout::getDescriptorSetLayouts() const {
    return *mDescriptorSetLayouts;
}

PipelineLayout::~PipelineLayout() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyPipelineLayout(*this);
}
