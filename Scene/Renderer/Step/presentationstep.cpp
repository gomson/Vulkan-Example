#include "presentationstep.hpp"

PresentationStep::PresentationStep(const Device &device, CommandBufferSubmitter commandBufferSubmitter,
                                   CommandPool drawCommandPool, vk::Queue queue, Transferer &transferer) :
    mDevice(std::make_shared<Device>(device)),
    mCommandBufferSubmitter(std::make_shared<CommandBufferSubmitter>(commandBufferSubmitter)),
    mDrawCommandPools(std::make_shared<CommandPool>(drawCommandPool)),
    mQueue(std::make_shared<vk::Queue>(queue))
{
    glm::vec2 quad[] = {glm::vec2(-1 , -1), glm::vec2(1, -1), glm::vec2(-1, 1), glm::vec2(1, 1)};
    *mVbo = Buffer(device, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, sizeof quad, transferer.getAllocator(), true);
    transferer.transfer(*mVbo, 0, sizeof quad, quad, vk::PipelineStageFlagBits::eVertexInput);
}

void PresentationStep::destroySwapchainKHR() {
    *mSwapchainKHR = SwapchainKHR();
}

void PresentationStep::rebuildSwapchainKHR(vk::SurfaceKHR surfaceKHR) {
    mDevice->waitIdle();
    *mSwapchainKHR = SwapchainKHR(*mDevice, surfaceKHR, *mRenderPass, *mSwapchainKHR);

    std::vector<vk::DescriptorPoolSize> poolSizes;
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    for(auto i(0u); i < mSwapchainKHR->getImageCount(); ++i) {
        descriptorSetLayouts.emplace_back(mPipeline->getLayout().getDescriptorSetLayouts()[0]);
        poolSizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, 1);
    }

    *mDescriptorPool = DescriptorPool(*mDevice, mSwapchainKHR->getImageCount(), {vk::DescriptorType::eCombinedImageSampler}, mPipeline->getLayout().getDescriptorSetLayouts()[0]);
    mDescriptorSets->resize(0);
    for(auto i(0u); i < mSwapchainKHR->getImageCount(); ++i)
        mDescriptorSets->emplace_back(mDescriptorPool->allocate());
}

void PresentationStep::updateImages(std::vector<CompleteFrameBuffer> framebuffer, uint32_t indexBackTexture) {
    for(auto i = 0u; i < framebuffer.size(); ++i) {
        vk::DescriptorImageInfo imageInfo(VK_NULL_HANDLE,
                                          framebuffer[i].getImageViews()[indexBackTexture],
                                          vk::ImageLayout::eShaderReadOnlyOptimal);

        vk::WriteDescriptorSet write((*mDescriptorSets)[i],
                0, 0, 1, vk::DescriptorType::eCombinedImageSampler,
                &imageInfo, nullptr, nullptr);
        mDevice->updateDescriptorSets(write, nullptr);
    }
    buildDrawCommandBuffers();
}

uint32_t PresentationStep::getCurrentIndex() {
    uint32_t index = mDevice->acquireNextImageKHR(*mSwapchainKHR, UINT64_MAX,
                                                  *mImageAvailableSemaphore, vk::Fence()).value;
    mCommandBufferSubmitter->setCurrentBatch(index);
    mCommandBufferSubmitter->wait();
    return index;
}

vk::CommandBuffer PresentationStep::buildPrimaryCommandBuffer(int index) {
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr);
    vk::CommandBuffer cmd = mCommandBufferSubmitter->createCommandBuffer(nullptr);
    cmd.begin(beginInfo);
    vk::ClearValue clear;
    clear.color = vk::ClearColorValue(std::array<float, 4>{{1, 0, 0, 1}});

    vk::RenderPassBeginInfo renderPassBeginInfo(*mRenderPass,
                                                mSwapchainKHR->getFrameBuffers(index),
                                                vk::Rect2D({0, 0}, {mSwapchainKHR->getWidth(), mSwapchainKHR->getHeight()}),
                                                1, &clear);

    cmd.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eSecondaryCommandBuffers);

    cmd.executeCommands((*mDrawCommandBuffers)[index]);

    cmd.endRenderPass();
    cmd.end();

    return cmd;
}

void PresentationStep::buildDrawCommandBuffers() {
    if(mDrawCommandBuffers->size() > 0)
        mDevice->freeCommandBuffers(*mDrawCommandPools, *mDrawCommandBuffers);
    *mDrawCommandBuffers = mDrawCommandPools->allocate(vk::CommandBufferLevel::eSecondary, mSwapchainKHR->getImageCount());

    for(auto i = 0u; i < mDrawCommandBuffers->size(); ++i) {
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
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                               mPipeline->getLayout(), 0,
                               (*mDescriptorSets)[i], {});
        cmd.draw(4, 1, 0, 0);
        cmd.end();
    }
}

void PresentationStep::execute(uint32_t index) {
    buildPrimaryCommandBuffer(index);

    vk::PipelineStageFlags semaphoreWait = vk::PipelineStageFlagBits::eTopOfPipe;

    mCommandBufferSubmitter->submit(1, mImageAvailableSemaphore.get(), &semaphoreWait,
                                    1, mImageRenderFinishedSemaphore.get());

    vk::PresentInfoKHR present(1, mImageRenderFinishedSemaphore.get(), 1,
                               mSwapchainKHR.get(), &index, nullptr);

    mQueue->presentKHR(present);
}

uint32_t PresentationStep::getNumberImages() const {
    return mSwapchainKHR->getImageCount();
}
