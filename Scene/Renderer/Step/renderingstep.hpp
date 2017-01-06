#pragma once
#include "VkTools/Command/commandpool.hpp"
#include "Renderer/FrameBuffer/renderingframebuffer.hpp"
#include "Renderer/Pipeline/staticgeometrypipeline.hpp"
#include "VkTools/Command/commandbuffersubmitter.hpp"
#include "VkTools/Command/transferer.hpp"

#include "staticgeometrystep.hpp"

class RenderingStep
{
public:
    RenderingStep(const Device &device, CommandBufferSubmitter commandBufferSubmitter, Transferer transferer);

    void resize(uint32_t width, uint32_t height, std::shared_ptr<AbstractAllocator> allocator, uint32_t numberRoundRobin);
    std::vector<CompleteFrameBuffer> getFramebuffer() const;

    void execute(uint32_t index);

private:
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<RenderingPass> mRenderPass = std::make_shared<RenderingPass>(*mDevice);
    std::shared_ptr<std::vector<RenderingFrameBuffer>> mFrameBuffers = std::make_shared<std::vector<RenderingFrameBuffer>>();
    std::shared_ptr<CommandBufferSubmitter> mCommandBufferSubmitter;
    std::shared_ptr<Transferer> mTransferer;

    std::shared_ptr<StaticGeometryStep> mStaticGeometryStep = std::make_shared<StaticGeometryStep>(*mDevice, *mRenderPass);
};
