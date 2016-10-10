#include "abstractpipeline.hpp"

AbstractPipeline::AbstractPipeline(Device &device, std::shared_ptr<AbstractPipelineLayout> pipelineLayout) :
    VkResource(device),
    mPipelineLayout(pipelineLayout) {

}

std::shared_ptr<AbstractPipelineLayout> AbstractPipeline::getLayout() {
    return mPipelineLayout;
}

AbstractPipeline::~AbstractPipeline() {
    mDevice.destroyPipeline(*this);
}
