#pragma once
#include "abstractrenderingstep.hpp"
#include "VkTools/Command/commandpool.hpp"
#include "Renderer/FrameBuffer/geometryframebuffer.hpp"
#include "Renderer/Pipeline/geometrypipeline.hpp"
#include "VkTools/Command/commandbuffersubmitter.hpp"

class GeometryStep : public AbstractRenderingStep
{
public:
    GeometryStep(const Device &device, CommandBufferSubmitter commandBufferSubmitter);

    void resize(uint32_t width, uint32_t height, std::shared_ptr<AbstractAllocator> allocator, uint32_t numberRoundRobin);
    vk::ArrayProxy<vk::ArrayProxy<ImageView>> getImages() const;

    void execute(uint32_t index);

private:
    std::shared_ptr<GeometryRenderPass> mRenderPass = std::make_shared<GeometryRenderPass>(*mDevice);
    std::shared_ptr<std::vector<GeometryFrameBuffer>> mFrameBuffers = std::make_shared<std::vector<GeometryFrameBuffer>>();
    std::shared_ptr<GeometryPipeline> mPipeline = std::make_shared<GeometryPipeline>(*mDevice, "../Shaders/geometry_static", *mRenderPass);
    std::shared_ptr<CommandBufferSubmitter> mCommandBufferSubmitter;
};
