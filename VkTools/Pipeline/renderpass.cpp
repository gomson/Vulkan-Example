#include "renderpass.hpp"

RenderPass::RenderPass(const Device &device) :
    VkResource(device)
{

}

RenderPass::~RenderPass() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice->destroyRenderPass(*this);
}
