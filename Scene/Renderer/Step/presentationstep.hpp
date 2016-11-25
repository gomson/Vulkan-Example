#pragma once
#include "VkTools/System/swapchain.hpp"
#include "VkTools/Synchronization/semaphore.hpp"
#include "VkTools/Synchronization/fence.hpp"
#include "VkTools/Command/commandpool.hpp"
#include "VkTools/Buffer/buffer.hpp"
#include "VkTools/Command/transferer.hpp"
#include "abstractrenderingstep.hpp"
#include "Renderer/RenderPass/renderpasstopresent.hpp"
#include "Renderer/Pipeline/pipelinetopresent.hpp"
#include "VkTools/Descriptor/descriptorpool.hpp"
#include "VkTools/Image/sampler.hpp"

class PresentationStep : public AbstractRenderingStep
{
public:
    PresentationStep(Device const &device, vk::SurfaceKHR surfaceKHR, CommandBufferSubmitter commandBufferSubmitter, CommandPool drawCommandPool, vk::Queue queue, Transferer &transferer);

    uint32_t getCurrentIndex();
    void destroySwapchainKHR();
    void rebuildSwapchainKHR(vk::SurfaceKHR surfaceKHR);
    void updateImages(vk::ArrayProxy<vk::ArrayProxy<ImageView>> images);

    uint32_t getNumberImages() const;

    void execute(uint32_t index);

private:
    std::shared_ptr<RenderPassToPresent> mRenderPass = std::make_shared<RenderPassToPresent>(*mDevice);
    std::shared_ptr<Sampler> mSampler = std::make_shared<Sampler>(*mDevice, 0.f);
    std::shared_ptr<PipelineToPresent> mPipeline = std::make_shared<PipelineToPresent>(*mDevice, *mRenderPass, mSampler);
    std::shared_ptr<SwapchainKHR> mSwapchainKHR = std::make_shared<SwapchainKHR>();
    std::shared_ptr<Semaphore> mImageAvailableSemaphore = std::make_shared<Semaphore>(*mDevice);
    std::shared_ptr<Semaphore> mImageRenderFinishedSemaphore = std::make_shared<Semaphore>(*mDevice);
    std::shared_ptr<CommandBufferSubmitter> mCommandBufferSubmitter;
    std::shared_ptr<CommandPool> mDrawCommandPools;
    std::shared_ptr<std::vector<vk::CommandBuffer>> mDrawCommandBuffers = std::make_shared<std::vector<vk::CommandBuffer>>();
    std::shared_ptr<vk::Queue> mQueue;

    std::shared_ptr<Buffer> mVbo = std::make_shared<Buffer>();
    std::shared_ptr<DescriptorPool> mDescriptorPool = std::make_shared<DescriptorPool>();
    std::shared_ptr<std::vector<vk::DescriptorSet>> mDescriptorSets = std::make_shared<std::vector<vk::DescriptorSet>>();

    vk::CommandBuffer buildPrimaryCommandBuffer(int index);
    void buildDrawCommandBuffers();
};
