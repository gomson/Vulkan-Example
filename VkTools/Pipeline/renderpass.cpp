#include "renderpass.hpp"

RenderPass::RenderPass(Device &device) :
    VkResource(device)
{

}

RenderPass::~RenderPass() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyRenderPass(*this);
}
