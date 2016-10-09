#include "abstractpipelinelayout.hpp"

AbstractPipelineLayout::AbstractPipelineLayout(Device &device) :
    VkResource(device)
{

}

AbstractPipelineLayout::~AbstractPipelineLayout() {
    mDevice.destroyPipelineLayout(*this);
}
