#pragma once
#include "abstractrenderingstep.hpp"
#include "Renderer/FrameBuffer/geometryframebuffer.hpp"

class GeometryStep : public AbstractRenderingStep
{
public:
    GeometryStep(const Device &device);

    void resize(uint32_t width, uint32_t height, std::shared_ptr<AbstractAllocator> allocator);

private:
    std::shared_ptr<GeometryRenderPass> mRenderPass = std::make_shared<GeometryRenderPass>(*mDevice);
    std::shared_ptr<GeometryFrameBuffer> mFrameBuffer;
};
