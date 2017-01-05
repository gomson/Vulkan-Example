#pragma once
#include "image.hpp"
#include "imageview.hpp"
#include "framebuffer.hpp"

class CompleteFrameBuffer : public FrameBuffer
{
public:
    CompleteFrameBuffer() = default;
    CompleteFrameBuffer(const Device &device, uint32_t width, uint32_t height);
    CompleteFrameBuffer(const CompleteFrameBuffer &frameBuffer) = default;
    CompleteFrameBuffer(CompleteFrameBuffer &&frameBuffer) = default;

    CompleteFrameBuffer &operator=(CompleteFrameBuffer &&frameBuffer) = default;
    CompleteFrameBuffer &operator=(const CompleteFrameBuffer &frameBuffer) = default;

    std::vector<ImageView> const getImageViews() const;
    std::vector<Image> const getImages() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;

protected:
    std::shared_ptr<std::vector<ImageView>> mImageViews = std::make_shared<std::vector<ImageView>>();
    std::shared_ptr<std::vector<Image>> mImages = std::make_shared<std::vector<Image>>();
    std::shared_ptr<uint32_t> mWidth = std::make_shared<uint32_t>();
    std::shared_ptr<uint32_t> mHeight = std::make_shared<uint32_t>();
};
