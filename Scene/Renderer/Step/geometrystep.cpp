#include "geometrystep.hpp"
#include "VkTools/Synchronization/fence.hpp"

GeometryStep::GeometryStep(const Device &device, CommandPool commandPool) :
    AbstractRenderingStep(device),
    mQueue(std::make_shared<vk::Queue>(device.getGraphicQueue())),
    mCommandPool(std::make_shared<CommandPool>(commandPool))
{

}

void GeometryStep::resize(uint32_t width, uint32_t height, std::shared_ptr<AbstractAllocator> allocator, uint32_t numberRoundRobin) {
    if(mCommandBuffer->size() > 0)
        mDevice->freeCommandBuffers(*mCommandPool, *mCommandBuffer);
    vk::CommandBufferAllocateInfo info(*mCommandPool, vk::CommandBufferLevel::ePrimary, numberRoundRobin);
    *mCommandBuffer = mDevice->allocateCommandBuffers(info);
    mFrameBuffers->resize(numberRoundRobin);
    for(auto &framebuffer : *mFrameBuffers)
        framebuffer = GeometryFrameBuffer(*mDevice, width, height, *mRenderPass,
                                          allocator);
}

vk::ArrayProxy<vk::ArrayProxy<ImageView>> GeometryStep::getImages() const {
    static std::vector<vk::ArrayProxy<ImageView>> imagesFromAllFramebuffers;
    imagesFromAllFramebuffers.resize(0, nullptr);
    for(uint32_t i = 0; i < mFrameBuffers->size(); ++i)
        imagesFromAllFramebuffers.emplace_back((*mFrameBuffers)[i].getImageViews());
    return imagesFromAllFramebuffers;
}

void GeometryStep::execute() {
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr);

    std::vector<vk::ClearValue> clears;
    std::array<float, 4> values = {1.f, 0.f, 1.f, 1.f};
    clears.emplace_back(vk::ClearColorValue(values));
    clears.emplace_back(vk::ClearColorValue(values));
    clears.emplace_back(vk::ClearColorValue(values));
    clears.emplace_back(vk::ClearDepthStencilValue(1.f, 0));

    vk::RenderPassBeginInfo renderPassBegin(*mRenderPass,
                                           (*mFrameBuffers)[*mIndex],
                                            vk::Rect2D({0, 0},
                                                     {(*mFrameBuffers)[*mIndex].getWidth(),
                                                      (*mFrameBuffers)[*mIndex].getHeight()}),
                                            clears.size(), clears.data());

    vk::CommandBuffer cmd = (*mCommandBuffer)[*mIndex];

    cmd.begin(beginInfo);
    cmd.beginRenderPass(renderPassBegin, vk::SubpassContents::eSecondaryCommandBuffers);

    cmd.endRenderPass();
    cmd.end();
    *mIndex = (*mIndex + 1) % mCommandBuffer->size();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    Fence fence(*mDevice, false);
    mQueue->submit({submitInfo}, {fence});
    fence.wait();
}
