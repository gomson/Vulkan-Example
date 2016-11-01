#include "imageloader.hpp"
#include <SDL2/SDL_image.h>

ImageLoader::ImageLoader(std::string const &path) {
    mImage = IMG_Load(path.c_str());

    if(mImage == nullptr)
        throw std::runtime_error(path + " " + IMG_GetError());

    SDL_Surface *t;
    SDL_PixelFormat format = *mImage->format;

    if(format.Rmask == 0x000000ff)
        format.format = SDL_PIXELFORMAT_RGBA8888;

    else
        format.format = SDL_PIXELFORMAT_BGRA8888;

    format.BitsPerPixel = 32;
    format.BytesPerPixel = 4;

    t = SDL_ConvertSurface(mImage, &format, SDL_SWSURFACE);

    SDL_FreeSurface(mImage);
    mImage = t;
}

uint32_t ImageLoader::getWidth() const {
    return mImage->w;
}

uint32_t ImageLoader::getHeight() const {
    return mImage->h;
}

uint32_t ImageLoader::getRowPitch() const {
    return mImage->pitch;
}

const unsigned char *ImageLoader::getPixel() const {
    return (unsigned char*)mImage->pixels;
}

vk::ImageCreateInfo ImageLoader::getImageGPUCreateInfo() const {
    uint32_t mipLevels = log2(std::max(getWidth(), getHeight())) + 1;

    vk::ImageCreateInfo info(vk::ImageCreateFlags(),
                             vk::ImageType::e2D, getFormat(),
                             vk::Extent3D(getWidth(), getHeight(), 1),
                             mipLevels, 1, vk::SampleCountFlagBits::e1,
                             vk::ImageTiling::eOptimal,
                             vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
                             vk::SharingMode::eExclusive, 0, nullptr,
                             vk::ImageLayout::eUndefined);

    return info;
}

vk::ImageCreateInfo ImageLoader::getImageCreateInfo() const {
    vk::ImageCreateInfo info(vk::ImageCreateFlags(),
                             vk::ImageType::e2D, getFormat(),
                             vk::Extent3D(getWidth(), getHeight(), 1),
                             1, 1, vk::SampleCountFlagBits::e1,
                             vk::ImageTiling::eLinear,
                             vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
                             vk::SharingMode::eExclusive, 0, nullptr,
                             vk::ImageLayout::ePreinitialized);

    return info;
}

vk::Format ImageLoader::getFormat() const {
    if(mImage->format->Rmask == 0x000000ff)
        return vk::Format::eR8G8B8A8Unorm;

     else
        return vk::Format::eB8G8R8A8Unorm;
}

ImageLoader::~ImageLoader() {
    SDL_FreeSurface(mImage);
}
