#pragma once
#include "VkTools/Image/image.hpp"
#include "VkTools/Image/imageview.hpp"
#include "VkTools/Image/sampler.hpp"
#include "VkTools/Command/transferer.hpp"

struct Texture
{
    Texture() = default;

    Texture(std::string const &path, Transferer &transferer) {
        Image::createImageFromPath(path, image, imageView, transferer);
        sampler = Sampler(transferer.getAllocator()->getDevice(),
                          std::floor(std::log2(std::max(image.getSize().width, image.getSize().height))));
    }

    Image image;
    ImageView imageView;
    Sampler sampler;
};
