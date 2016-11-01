#pragma once
#include "vulkan/vulkan.hpp"

class SDL_Surface;
class ImageLoader : private NotCopyable
{
public:
    ImageLoader(std::string const &path);

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getRowPitch() const;

    unsigned char const *getPixel() const;

    vk::ImageCreateInfo getImageCreateInfo() const;
    vk::ImageCreateInfo getImageGPUCreateInfo() const;

    ~ImageLoader();

private:
    SDL_Surface *mImage;
    vk::Format mFormat;

    vk::Format getFormat() const;
};
