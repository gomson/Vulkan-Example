#include "pipeline.hpp"

Pipeline::Pipeline(Device &device, PipelineLayout pipelineLayout) :
    VkResource(device),
    mPipelineLayout(std::make_shared<PipelineLayout>(pipelineLayout)) {

}

PipelineLayout Pipeline::getLayout() {
    return *mPipelineLayout;
}

Pipeline::~Pipeline() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyPipeline(*this);
}
