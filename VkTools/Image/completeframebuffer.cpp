#include "completeframebuffer.hpp"

CompleteFrameBuffer::CompleteFrameBuffer(const Device &device, uint32_t width, uint32_t height) :
    FrameBuffer(device) {
    *mWidth = width;
    *mHeight = height;
}

const std::vector<ImageView> CompleteFrameBuffer::getImageViews() const {
    return *mImageViews;
}

const std::vector<Image> CompleteFrameBuffer::getImages() const {
    return *mImages;
}

uint32_t CompleteFrameBuffer::getWidth() const {
    return *mWidth;
}

uint32_t CompleteFrameBuffer::getHeight() const {
    return *mHeight;
}
