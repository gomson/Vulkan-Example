#pragma once
#include "VkTools/Image/image.hpp"
#include "VkTools/Image/imageview.hpp"
#include "VkTools/Image/framebuffer.hpp"
#include "Renderer/RenderPass/geometryrenderpass.hpp"

class GeometryFrameBuffer : public FrameBuffer
{
public:
    GeometryFrameBuffer(const Device &device, uint32_t width, uint32_t height,
                        const GeometryRenderPass &geometryRenderPass,
                        std::shared_ptr<AbstractAllocator> allocator);

private:
    std::shared_ptr<AbstractAllocator> mAllocator;
    std::shared_ptr<std::vector<ImageView>> mImageViews = std::make_shared<std::vector<ImageView>>();
    std::shared_ptr<std::vector<Image>> mImages = std::make_shared<std::vector<Image>>();
};
