#include "image.hpp"
#include "../System/imageview.hpp"
#include "../Memory/imageloader.hpp"
#include "../Memory/bufferimagetransferer.hpp"

void swap(Image &i1, Image &i2) {
    using std::swap;
    swap(static_cast<VkResource&>(i1), static_cast<VkResource&>(i2));
    swap(i1.mAllocator, i2.mAllocator);
    swap(i1.mSize, i2.mSize);
    swap(i1.mMipLevels, i2.mMipLevels);
    swap(i1.mArrayLayers, i2.mArrayLayers);
    swap(i1.mUsage, i2.mUsage);
    swap(i1.mFormat, i2.mFormat);
    swap(i1.mBlock, i2.mBlock);
    swap(i1.mImageType, i2.mImageType);
    swap(i1.mPtr, i2.mPtr);
    swap(i1.m_image, i2.m_image);
}

Image::Image(Image &&image) :
    VkResource(image),
    vk::Image(image) {
    swap(*this, image);
}

Image::Image(const Image &image) :
    VkResource(image),
    vk::Image(image),
    mAllocator(image.mAllocator),
    mSize(image.mSize),
    mMipLevels(image.mMipLevels),
    mArrayLayers(image.mArrayLayers),
    mUsage(image.mUsage),
    mFormat(image.mFormat),
    mImageType(image.mImageType),
    mBlock(image.mBlock),
    mPtr(image.mPtr) {

}

Image &Image::operator =(Image image) {
    swap(*this, image);
    return *this;
}

Image::Image(Device const &device, vk::ImageCreateInfo info, std::shared_ptr<AbstractAllocator> allocator) :
    VkResource(device),
    mAllocator(allocator),
    mSize(std::make_shared<vk::Extent3D>(info.extent)),
    mMipLevels(std::make_shared<uint32_t>(info.mipLevels)),
    mArrayLayers(std::make_shared<uint32_t>(info.arrayLayers)),
    mUsage(std::make_shared<vk::ImageUsageFlags>(info.usage)),
    mFormat(std::make_shared<vk::Format>(info.format)),
    mImageType(std::make_shared<vk::ImageType>(info.imageType)) {
    m_image = device.createImage(info);

    vk::MemoryRequirements requirements = device.getImageMemoryRequirements(m_image);
    bool inGPU = info.tiling == vk::ImageTiling::eOptimal ? true : false;
    auto index = findMemoryType(requirements.memoryTypeBits, device.getPhysicalDevice().getMemoryProperties(), inGPU);
    *mBlock = allocator->allocate(requirements.size, index);

    if(inGPU == false)
        *mPtr = device.mapMemory(mBlock->memory, 0, VK_WHOLE_SIZE, vk::MemoryMapFlags());

    device.bindImageMemory(m_image, mBlock->memory, mBlock->offset);
}

void Image::pushData(const unsigned char *data, uint32_t rowPitch) {
    assert(mPtr != nullptr);

    unsigned char *ptr = (unsigned char*) *mPtr;

    vk::ImageSubresource subResource;
    subResource.aspectMask = vk::ImageAspectFlagBits::eColor;
    subResource.mipLevel = 0;
    subResource.arrayLayer = 0;

    vk::SubresourceLayout layout = mDevice.getImageSubresourceLayout(m_image, subResource);

    for(uint32_t i = 0; i < mSize->height; ++i) {
        memcpy(ptr, data, rowPitch);
        ptr += layout.rowPitch;
        data += rowPitch;
    }
}

vk::Extent3D Image::getSize() const {
    return *mSize;
}

uint32_t Image::getMipLevels() const {
    return *mMipLevels;
}

uint32_t Image::getArrayLayers() const {
    return *mArrayLayers;
}

vk::ImageUsageFlags Image::getUsage() const {
    return *mUsage;
}

vk::Format Image::getFormat() const {
    return *mFormat;
}

vk::ImageType Image::getImageType() const {
    return *mImageType;
}

vk::ImageViewCreateInfo Image::getImageViewCreateInfo(vk::ImageAspectFlags aspect, bool arrayed, bool cube) const {
    vk::ImageViewType imageViewType;

    if(getImageType() == vk::ImageType::e1D) {
        if(arrayed)
            imageViewType = vk::ImageViewType::e1DArray;
        else
            imageViewType = vk::ImageViewType::e1D;
    }

    else if(getImageType() == vk::ImageType::e2D) {
        if(arrayed) {
            if(cube)
                imageViewType = vk::ImageViewType::eCubeArray;

            else
                imageViewType = vk::ImageViewType::e2DArray;
        }

        else {
            if(cube)
                imageViewType = vk::ImageViewType::eCube;

            else
                imageViewType = vk::ImageViewType::e2D;
        }
    }

    else if(getImageType() == vk::ImageType::e3D)
        imageViewType = vk::ImageViewType::e3D;

    else
        assert(!"Image Type not managed");

    vk::ImageSubresourceRange range(aspect, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS);

    vk::ImageViewCreateInfo info(vk::ImageViewCreateFlags(),
                                 m_image, imageViewType,
                                 getFormat(), vk::ComponentMapping(), range);
    return info;
}

void Image::createImageFromPath(const std::string &path, Image &image, ImageView &imageView,
                                BufferImageTransferer &bufferImageTransferer,
                                std::shared_ptr<AbstractAllocator> allocator) {
    ImageLoader loader(path);
    Image imageCPU(allocator->getDevice(), loader.getImageCreateInfo(), allocator);
    image = Image(allocator->getDevice(), loader.getImageGPUCreateInfo(), allocator);

    imageCPU.pushData(loader.getPixel(), loader.getRowPitch());

    bufferImageTransferer.transfer(imageCPU, image,
                                   vk::ImageLayout::ePreinitialized,
                                   vk::ImageLayout::eUndefined,
                                   vk::ImageLayout::eShaderReadOnlyOptimal,
                                   vk::ImageLayout::eTransferSrcOptimal,
                                   vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1),
                                   vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1),
                                   vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
                                   vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
                                   vk::Offset3D(0, 0, 0), vk::Offset3D(0, 0, 0),
                                   image.getSize());

    imageView = ImageView(allocator->getDevice(), image.getImageViewCreateInfo(vk::ImageAspectFlagBits::eColor, false, false));
    bufferImageTransferer.flush();

    bufferImageTransferer.buildMipMap(image);
    bufferImageTransferer.flush();
}

Image::~Image() {
    if((VkDevice)mDevice != VK_NULL_HANDLE && mCount != nullptr && --(*mCount) == 0) {
        mAllocator->deallocate(*mBlock);
        mDevice.destroyImage(m_image);
    }
}
