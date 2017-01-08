#include "renderingstep.hpp"
#include "VkTools/Synchronization/fence.hpp"

RenderingStep::RenderingStep(const Device &device, CommandBufferSubmitter commandBufferSubmitter, Transferer transferer) :
    mDevice(std::make_shared<Device>(device)),
    mCommandBufferSubmitter(std::make_shared<CommandBufferSubmitter>(commandBufferSubmitter)),
    mTransferer(std::make_shared<Transferer>(transferer))
{
}

void RenderingStep::resize(uint32_t width, uint32_t height, std::shared_ptr<AbstractAllocator> allocator, uint32_t numberRoundRobin) {
    mFrameBuffers->resize(numberRoundRobin);
    for(auto &framebuffer : *mFrameBuffers)
        framebuffer = RenderingFrameBuffer(*mDevice, width, height, *mRenderPass,
                                           allocator);
    mStaticGeometryStep->compileScene(*mFrameBuffers);
}

std::vector<CompleteFrameBuffer> RenderingStep::getFramebuffer() const {
    return *mFrameBuffers;
}

void RenderingStep::execute(uint32_t index) {
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr);

    std::vector<vk::ClearValue> clears;
    std::array<float, 4> values{{0.f, 0.f, 0.f, 1.f}};
    clears.emplace_back(vk::ClearColorValue(values));
    clears.emplace_back(vk::ClearDepthStencilValue(1.f, 0));

    vk::RenderPassBeginInfo renderPassBegin(*mRenderPass,
                                           (*mFrameBuffers)[index],
                                            vk::Rect2D({0, 0},
                                                     {(*mFrameBuffers)[index].getWidth(),
                                                      (*mFrameBuffers)[index].getHeight()}),
                                            clears.size(), clears.data());

    vk::CommandBuffer cmd = mCommandBufferSubmitter->createCommandBuffer(nullptr);

    cmd.begin(beginInfo);
    cmd.beginRenderPass(renderPassBegin, vk::SubpassContents::eSecondaryCommandBuffers);

    cmd.executeCommands(mStaticGeometryStep->getCommandBuffer(index));

    cmd.endRenderPass();
    cmd.end();
}
