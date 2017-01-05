#pragma once
#include "VkTools/Image/image.hpp"
#include "VkTools/Image/imageview.hpp"
#include "VkTools/Image/completeframebuffer.hpp"
#include "Renderer/RenderPass/renderingpass.hpp"

class RenderingFrameBuffer : public CompleteFrameBuffer
{
public:
    RenderingFrameBuffer() = default;
    RenderingFrameBuffer(const Device &device, uint32_t width, uint32_t height,
                        const RenderingPass &geometryRenderPass,
                        std::shared_ptr<AbstractAllocator> allocator);

    RenderingFrameBuffer(const RenderingFrameBuffer &frameBuffer) = default;
    RenderingFrameBuffer(RenderingFrameBuffer &&frameBuffer) = default;

    RenderingFrameBuffer &operator=(RenderingFrameBuffer &&geometryFrameBuffer) = default;
    RenderingFrameBuffer &operator=(RenderingFrameBuffer const &geometryFrameBuffer) = default;

private:
};
