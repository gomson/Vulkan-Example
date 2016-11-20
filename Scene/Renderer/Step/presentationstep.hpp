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

class PresentationStep : AbstractRenderingStep
{
public:
    PresentationStep(Device const &device, vk::SurfaceKHR surfaceKHR, CommandPool transientCommandPool, CommandPool drawCommandPool, vk::Queue queue, Transferer &transferer);

    void destroySwapchainKHR();
    void rebuildSwapchainKHR(vk::SurfaceKHR surfaceKHR);

    void execute();

private:
    std::shared_ptr<RenderPassToPresent> mRenderPass = std::make_shared<RenderPassToPresent>(*mDevice);
    std::shared_ptr<PipelineToPresent> mPipeline = std::make_shared<PipelineToPresent>(*mDevice, *mRenderPass);
    std::shared_ptr<SwapchainKHR> mSwapchainKHR = std::make_shared<SwapchainKHR>();
    std::shared_ptr<Semaphore> mImageAvailableSemaphore = std::make_shared<Semaphore>(*mDevice);
    std::shared_ptr<Semaphore> mImageRenderFinishedSemaphore = std::make_shared<Semaphore>(*mDevice);
    std::shared_ptr<CommandPool> mTransientCommandPool;
    std::shared_ptr<CommandPool> mDrawCommandPools;
    std::shared_ptr<std::vector<vk::CommandBuffer>> mPrimaryCommandBuffers = std::make_shared<std::vector<vk::CommandBuffer>>();
    std::shared_ptr<std::vector<vk::CommandBuffer>> mDrawCommandBuffers = std::make_shared<std::vector<vk::CommandBuffer>>();
    std::shared_ptr<std::vector<Fence>> mFences = std::make_shared<std::vector<Fence>>();
    std::shared_ptr<vk::Queue> mQueue;

    std::shared_ptr<Buffer> mVbo = std::make_shared<Buffer>();

    void buildPrimaryCommandBuffer(int index);
    void buildDrawCommandBuffers();
};
