#pragma once
#include "vulkan/vkresource.hpp"
#include "abstractallocator.hpp"

class ImageTransferer;
class ImageView;

class Image : public VkResource, public vk::Image
{
public:
    Image() = default;
    Image(Image &&image) = default;
    Image(Image const &image) = default;
    Image &operator=(Image const &image) = default;
    Image(Device const &device, vk::ImageCreateInfo info, std::shared_ptr<AbstractAllocator> allocator);

    vk::Extent3D getSize() const;
    uint32_t getMipLevels() const;
    uint32_t getArrayLayers() const;
    vk::ImageUsageFlags getUsage() const;
    vk::Format getFormat() const;
    vk::ImageType getImageType() const;

    void pushData(unsigned char const *data, uint32_t rowPitch);

    vk::ImageViewCreateInfo getImageViewCreateInfo(vk::ImageAspectFlags aspect, bool arrayed, bool cube) const;

    static void createImageFromPath(std::string const &path, Image &image, ImageView &imageView,
                                    ImageTransferer &bufferImageTransferer,
                                    std::shared_ptr<AbstractAllocator> allocator );

    ~Image();

private:
    std::shared_ptr<AbstractAllocator> mAllocator;
    std::shared_ptr<vk::Extent3D> mSize;
    std::shared_ptr<uint32_t> mMipLevels;
    std::shared_ptr<uint32_t> mArrayLayers;
    std::shared_ptr<vk::ImageUsageFlags> mUsage;
    std::shared_ptr<vk::Format> mFormat;
    std::shared_ptr<vk::ImageType> mImageType;
    std::shared_ptr<Block> mBlock = std::make_shared<Block>();
    std::shared_ptr<void *> mPtr = std::make_shared<void*>(nullptr);
};
