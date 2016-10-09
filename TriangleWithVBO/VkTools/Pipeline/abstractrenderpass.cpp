#include "abstractrenderpass.hpp"

AbstractRenderPass::AbstractRenderPass(Device &device) :
    VkResource(device)
{

}

AbstractRenderPass::~AbstractRenderPass() {
    mDevice.destroyRenderPass(*this);
}
