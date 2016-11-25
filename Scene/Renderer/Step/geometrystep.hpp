#pragma once
#include "abstractrenderingstep.hpp"
#include "VkTools/Command/commandpool.hpp"
#include "Renderer/FrameBuffer/geometryframebuffer.hpp"
#include "Renderer/Pipeline/geometrypipeline.hpp"

class GeometryStep : public AbstractRenderingStep
{
public:
    GeometryStep(const Device &device, CommandPool commandPool);

    void resize(uint32_t width, uint32_t height, std::shared_ptr<AbstractAllocator> allocator, uint32_t numberRoundRobin);
    vk::ArrayProxy<vk::ArrayProxy<ImageView>> getImages() const;

    void execute();

private:
    std::shared_ptr<vk::Queue> mQueue;
    std::shared_ptr<GeometryRenderPass> mRenderPass = std::make_shared<GeometryRenderPass>(*mDevice);
    std::shared_ptr<std::vector<GeometryFrameBuffer>> mFrameBuffers = std::make_shared<std::vector<GeometryFrameBuffer>>();
    std::shared_ptr<GeometryPipeline> mPipeline = std::make_shared<GeometryPipeline>(*mDevice, "../Shaders/geometry_static", *mRenderPass);
    std::shared_ptr<CommandPool> mCommandPool;
    std::shared_ptr<std::vector<vk::CommandBuffer>> mCommandBuffer = std::make_shared<std::vector<vk::CommandBuffer>>();
    std::shared_ptr<uint32_t> mIndex = std::make_shared<uint32_t>(0);
};
