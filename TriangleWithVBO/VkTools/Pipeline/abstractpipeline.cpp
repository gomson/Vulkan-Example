#include "abstractpipeline.hpp"

AbstractPipeline::AbstractPipeline(Device &device) :
    VkResource(device) {

}

AbstractPipeline::~AbstractPipeline() {
    mDevice.destroyPipeline(*this);
}
