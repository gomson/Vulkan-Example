#include "staticgeometrystep.hpp"

StaticGeometryStep::StaticGeometryStep(Device &device, RenderingPass &renderpass, Transferer &transferer, DescriptorSetLayout materialLayout) :
    mDevice(std::make_shared<Device>(device)),
    mRenderingPass(std::make_shared<RenderingPass>(renderpass)),
    mTransferer(std::make_shared<Transferer>(transferer)),
    mPipeline(std::make_shared<StaticGeometryPipeline>(device, "../Shaders/geometry_static", renderpass, materialLayout))
{
}

void StaticGeometryStep::compileScene(const std::vector<CompleteFrameBuffer> &framebuffers, uint32_t index) {
    assert(index < framebuffers.size());

    if(mCommandBuffer->size() != framebuffers.size())
        *mCommandBuffer = mCommandPool->allocate(vk::CommandBufferLevel::eSecondary, framebuffers.size());

    glm::mat4 perspective = glm::perspective(glm::radians(70.f), (float)framebuffers[0].getWidth() / framebuffers[0].getHeight(), 1.f, 20000.f);
    perspective[1][1] *= -1;
    perspective *= glm::lookAt(glm::vec3(300, 500, 0), glm::vec3(0, 400, 0), glm::vec3(0, 1, 0));

    auto i = index;
    vk::CommandBufferInheritanceInfo inheritance(*mRenderingPass, 0, framebuffers[i]);
    vk::CommandBufferBeginInfo info(vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritance);

    (*mCommandBuffer)[i].begin(info);
    (*mCommandBuffer)[i].bindPipeline(vk::PipelineBindPoint::eGraphics, *mPipeline);
    (*mCommandBuffer)[i].bindVertexBuffers(0, *mVbo, {0u});
    (*mCommandBuffer)[i].setViewport(0, vk::Viewport(0, 0, framebuffers[i].getWidth(), framebuffers[i].getHeight(), 0.f, 1.f));
    (*mCommandBuffer)[i].setScissor(0, vk::Rect2D({0, 0}, {framebuffers[i].getWidth(), framebuffers[i].getHeight()}));
    (*mCommandBuffer)[i].bindIndexBuffer(*mIbo, 0, vk::IndexType::eUint32);
    mRootNode->draw((*mCommandBuffer)[i], mPipeline->getLayout(), vk::ShaderStageFlagBits::eVertex, perspective);
    (*mCommandBuffer)[i].end();
}

vk::CommandBuffer StaticGeometryStep::getCommandBuffer(uint32_t index) const {
    return (*mCommandBuffer)[index];
}
