#include "presentationstep.hpp"

PresentationStep::PresentationStep(const Device &device, vk::SurfaceKHR surfaceKHR, CommandPool transientCommandPool,
                                   CommandPool drawCommandPool, vk::Queue queue, Transferer &transferer) :
    AbstractRenderingStep(device),
    mTransientCommandPool(std::make_shared<CommandPool>(transientCommandPool)),
    mDrawCommandPools(std::make_shared<CommandPool>(drawCommandPool)),
    mQueue(std::make_shared<vk::Queue>(queue))
{
    glm::vec2 quad[] = {glm::vec2(-1 , -1), glm::vec2(1, -1), glm::vec2(-1, 1), glm::vec2(1, 1)};
    *mVbo = Buffer(device, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, sizeof quad, transferer.getAllocator(), true);
    transferer.transfer(*mVbo, 0, sizeof quad, quad);
    rebuildSwapchainKHR(surfaceKHR);
}

void PresentationStep::destroySwapchainKHR() {
    *mSwapchainKHR = SwapchainKHR();
}

void PresentationStep::rebuildSwapchainKHR(vk::SurfaceKHR surfaceKHR) {
    mDevice->waitIdle();
    *mSwapchainKHR = SwapchainKHR(*mDevice, surfaceKHR, *mRenderPass, *mSwapchainKHR);

    mFences->resize(mSwapchainKHR->getImageCount());
    for(auto &fence : *mFences)
        fence = Fence(*mDevice, true);

    if(mPrimaryCommandBuffers->size() > 0)
        mDevice->freeCommandBuffers(*mTransientCommandPool, *mPrimaryCommandBuffers);
    *mPrimaryCommandBuffers = mTransientCommandPool->allocate(vk::CommandBufferLevel::ePrimary, mSwapchainKHR->getImageCount());
    buildDrawCommandBuffers();
}

void PresentationStep::buildPrimaryCommandBuffer(int index) {
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr);
    (*mPrimaryCommandBuffers)[index].begin(beginInfo);
    vk::ClearValue clear;
    clear.color = vk::ClearColorValue(std::array<float, 4>{{1, 0, 0, 1}});

    vk::RenderPassBeginInfo renderPassBeginInfo(*mRenderPass,
                                                mSwapchainKHR->getFrameBuffers(index),
                                                vk::Rect2D({0, 0}, {mSwapchainKHR->getWidth(), mSwapchainKHR->getHeight()}),
                                                1, &clear);

    (*mPrimaryCommandBuffers)[index].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eSecondaryCommandBuffers);

    (*mPrimaryCommandBuffers)[index].executeCommands((*mDrawCommandBuffers)[index]);

    (*mPrimaryCommandBuffers)[index].endRenderPass();
    (*mPrimaryCommandBuffers)[index].end();
}

void PresentationStep::buildDrawCommandBuffers() {
    if(mDrawCommandBuffers->size() > 0)
        mDevice->freeCommandBuffers(*mDrawCommandPools, *mDrawCommandBuffers);
    *mDrawCommandBuffers = mDrawCommandPools->allocate(vk::CommandBufferLevel::eSecondary, mSwapchainKHR->getImageCount());

    for(int i = 0; i < mDrawCommandBuffers->size(); ++i) {
        vk::CommandBuffer cmd = (*mDrawCommandBuffers)[i];
        vk::CommandBufferInheritanceInfo inheritance(*mRenderPass, 0,
                                                     mSwapchainKHR->getFrameBuffers(i));
        vk::CommandBufferBeginInfo begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritance);
        cmd.begin(begin);
        vk::Viewport viewPort(0, 0,
                              mSwapchainKHR->getWidth(), mSwapchainKHR->getHeight(),
                              0, 1);
        vk::Rect2D scissor(vk::Offset2D(0, 0),
                           vk::Extent2D(mSwapchainKHR->getWidth(),
                                        mSwapchainKHR->getHeight()));
        cmd.setViewport(0, viewPort);
        cmd.setScissor(0, scissor);
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *mPipeline);
        cmd.bindVertexBuffers(0, *mVbo, vk::DeviceSize(0));
        cmd.draw(4, 1, 0, 0);
        cmd.end();
    }
}

void PresentationStep::execute() {
    uint32_t index = mDevice->acquireNextImageKHR(*mSwapchainKHR, UINT64_MAX, *mImageAvailableSemaphore, vk::Fence()).value;

    (*mFences)[index].wait();
    (*mFences)[index].reset();

    buildPrimaryCommandBuffer(index);

    vk::PipelineStageFlags semaphoreWait = vk::PipelineStageFlagBits::eTopOfPipe;

    vk::SubmitInfo si(1, mImageAvailableSemaphore.get(),
                      &semaphoreWait,
                      1, &(*mPrimaryCommandBuffers)[index],
                      1, mImageRenderFinishedSemaphore.get());

    vk::PresentInfoKHR present(1, mImageRenderFinishedSemaphore.get(), 1,
                               mSwapchainKHR.get(), &index, nullptr);

    mQueue->submit(si, (*mFences)[index]);
    mQueue->presentKHR(present);
}
