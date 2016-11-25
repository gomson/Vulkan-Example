#include "completeframebuffer.hpp"

CompleteFrameBuffer::CompleteFrameBuffer(const Device &device, uint32_t width, uint32_t height) :
    FrameBuffer(device) {
    *mWidth = width;
    *mHeight = height;
}

vk::ArrayProxy<ImageView> CompleteFrameBuffer::getImageViews() const {
    return *mImageViews;
}

uint32_t CompleteFrameBuffer::getWidth() const {
    return *mWidth;
}

uint32_t CompleteFrameBuffer::getHeight() const {
    return *mHeight;
}
