#include "pipeline.hpp"

void swap(Pipeline &p1, Pipeline &p2) {
    using std::swap;
    swap(static_cast<VkResource&>(p1), static_cast<VkResource&>(p2));
    swap(p1.mPipelineLayout, p2.mPipelineLayout);
    swap(p1.m_pipeline, p2.m_pipeline);
}

Pipeline::Pipeline(Pipeline &&pipeline) :
    VkResource(pipeline),
    vk::Pipeline(pipeline) {
    swap(*this, pipeline);
}

Pipeline::Pipeline(const Pipeline &pipeline) :
    VkResource(pipeline),
    vk::Pipeline(pipeline) {
    mPipelineLayout = pipeline.mPipelineLayout;
}

Pipeline &Pipeline::operator=(Pipeline pipeline) {
    swap(*this, pipeline);
    return *this;
}

Pipeline::Pipeline(Device &device, PipelineLayout pipelineLayout) :
    VkResource(device),
    mPipelineLayout(std::make_shared<PipelineLayout>(pipelineLayout)) {

}

PipelineLayout Pipeline::getLayout() {
    return *mPipelineLayout;
}

Pipeline::~Pipeline() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice.destroyPipeline(*this);
}
