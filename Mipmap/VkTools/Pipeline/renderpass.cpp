#include "renderpass.hpp"

void swap(RenderPass &r1, RenderPass &r2) {
    using std::swap;
    swap(static_cast<VkResource&>(r1), static_cast<VkResource&>(r2));
    swap(r1.m_renderPass, r2.m_renderPass);
}

RenderPass::RenderPass(RenderPass &&renderPass) :
    VkResource(renderPass),
    vk::RenderPass(renderPass) {
    swap(*this, renderPass);
}

RenderPass::RenderPass(const RenderPass &renderPass) :
    VkResource(renderPass),
    vk::RenderPass(renderPass) {

}

RenderPass &RenderPass::operator=(RenderPass renderPass) {
    swap(*this, renderPass);
    return *this;
}

RenderPass::RenderPass(Device &device) :
    VkResource(device)
{

}

RenderPass::~RenderPass() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice.destroyRenderPass(*this);
}
