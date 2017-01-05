#pragma once
#include "VkTools/Image/image.hpp"
#include "VkTools/Image/imageview.hpp"
#include "VkTools/Image/completeframebuffer.hpp"
#include "Renderer/RenderPass/renderingpass.hpp"

class GeometryFrameBuffer : public CompleteFrameBuffer
{
public:
    GeometryFrameBuffer() = default;
    GeometryFrameBuffer(const Device &device, uint32_t width, uint32_t height,
                        const RenderingPass &geometryRenderPass,
                        std::shared_ptr<AbstractAllocator> allocator);

    GeometryFrameBuffer(const GeometryFrameBuffer &frameBuffer) = default;
    GeometryFrameBuffer(GeometryFrameBuffer &&frameBuffer) = default;

    GeometryFrameBuffer &operator=(GeometryFrameBuffer &&geometryFrameBuffer) = default;
    GeometryFrameBuffer &operator=(GeometryFrameBuffer const &geometryFrameBuffer) = default;

private:
};
