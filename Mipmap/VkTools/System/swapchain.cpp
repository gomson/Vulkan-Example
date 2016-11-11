#include <iostream>
#include "swapchain.hpp"

void swap(SwapchainKHR &s1, SwapchainKHR &s2) {
    using std::swap;
    swap(static_cast<VkResource&>(s1), static_cast<VkResource&>(s2));
    swap(s1.mFormat, s2.mFormat);
    swap(s1.mFrameBuffer, s2.mFrameBuffer);
    swap(s1.mHeight, s2.mHeight);
    swap(s1.mImageCount, s2.mImageCount);
    swap(s1.mImages, s2.mImages);
    swap(s1.mImageView, s2.mImageView);
    swap(s1.mRenderPass, s2.mRenderPass);
    swap(s1.mWidth, s2.mWidth);
    swap(s1.m_swapchainKHR, s2.m_swapchainKHR);
}

SwapchainKHR::SwapchainKHR(SwapchainKHR &&swapchainKHR) :
    VkResource(swapchainKHR),
    vk::SwapchainKHR(swapchainKHR) {
    swap(*this, swapchainKHR);
}

SwapchainKHR::SwapchainKHR(const SwapchainKHR &swapchainKHR) :
    VkResource(swapchainKHR),
    vk::SwapchainKHR(swapchainKHR) {
    mFormat = swapchainKHR.mFormat;
    mFrameBuffer = swapchainKHR.mFrameBuffer;
    mHeight = swapchainKHR.mHeight;
    mImageCount = swapchainKHR.mImageCount;
    mImages = swapchainKHR.mImages;
    mImageView = swapchainKHR.mImageView;
    mRenderPass = swapchainKHR.mRenderPass;
    mWidth = swapchainKHR.mWidth;
}

SwapchainKHR &SwapchainKHR::operator =(SwapchainKHR swapchainKHR) {
    swap(*this, swapchainKHR);
    return *this;
}

SwapchainKHR::SwapchainKHR(Device &device, vk::SurfaceKHR surfaceKHR, RenderPass &renderPass,
                           vk::SwapchainKHR oldSwapchainKHR) :
    VkResource(device),
    mImages(std::make_shared<std::vector<vk::Image>>()),
    mFrameBuffer(std::make_shared<std::vector<FrameBuffer>>()),
    mImageView(std::make_shared<std::vector<ImageView>>()),
    mImageCount(std::make_shared<uint32_t>(0)),
    mRenderPass(std::make_shared<RenderPass>(renderPass)),
    mFormat(std::make_shared<vk::SurfaceFormatKHR>()),
    mWidth(std::make_shared<uint32_t>(0)),
    mHeight(std::make_shared<uint32_t>(0)) {
    std::vector<vk::SurfaceFormatKHR> formats(device.getPhysicalDevice().getSurfaceFormatsKHR(surfaceKHR));

    for(auto &fmt : formats) {
        if(fmt.format == vk::Format::eB8G8R8A8Unorm ||
           fmt.format == vk::Format::eUndefined) {
            *mFormat = vk::SurfaceFormatKHR(vk::Format::eB8G8R8A8Unorm);
            break;
        }
    }

    std::vector<vk::PresentModeKHR> modes(device.getPhysicalDevice().getSurfacePresentModesKHR(surfaceKHR));

    vk::PresentModeKHR mode = vk::PresentModeKHR::eFifo;
    for(auto &m : modes)
        if(m == vk::PresentModeKHR::eMailbox)
            mode = vk::PresentModeKHR::eMailbox;

    m_swapchainKHR = VK_NULL_HANDLE;

    m_swapchainKHR = mDevice.createSwapchainKHR(buildCreateInfos(surfaceKHR, mode, oldSwapchainKHR));
    *mImages = mDevice.getSwapchainImagesKHR(*this);
    *mImageCount = mImages->size();
    createImageViews();
    createFrameBuffers();
}


FrameBuffer const &SwapchainKHR::getFrameBuffers(uint32_t index) const {
    return (*mFrameBuffer)[index];
}

unsigned SwapchainKHR::getImageCount() const {
    return *mImageCount;
}

unsigned SwapchainKHR::getWidth() const {
    return *mWidth;
}

unsigned SwapchainKHR::getHeight() const {
    return *mHeight;
}

void SwapchainKHR::createImageViews() {
    mImageView->clear();

    for(vk::Image image : *mImages) {
        vk::ImageViewCreateInfo info(vk::ImageViewCreateFlags(),
                                     image, vk::ImageViewType::e2D,
                                     mFormat->format, vk::ComponentMapping(),
                                     vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
        mImageView->emplace_back(mDevice, info);
    }
}

void SwapchainKHR::createFrameBuffers() {
    mFrameBuffer->clear();
    for(auto &imageView : *mImageView) {
        vk::FramebufferCreateInfo fb(vk::FramebufferCreateFlags(),
                                     *mRenderPass, 1, &imageView,
                                     *mWidth, *mHeight, 1);

        mFrameBuffer->emplace_back(mDevice, fb);
    }
}

vk::SwapchainCreateInfoKHR SwapchainKHR::buildCreateInfos(vk::SurfaceKHR surfaceKHR, vk::PresentModeKHR mode,
                                                          vk::SwapchainKHR oldSwapchainKHR) {
    vk::SurfaceCapabilitiesKHR capabilities = mDevice.getPhysicalDevice().getSurfaceCapabilitiesKHR(surfaceKHR);
    vk::Extent2D extent(capabilities.currentExtent);
    *mWidth = extent.width;
    *mHeight = extent.height;

    auto minImageCount = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);

    vk::SwapchainCreateInfoKHR info(vk::SwapchainCreateFlagsKHR(), surfaceKHR, minImageCount,
                                    mFormat->format, mFormat->colorSpace, extent, 1,
                                    vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive,
                                    0, nullptr, capabilities.currentTransform,
                                    vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                    mode, true, oldSwapchainKHR);

    return info;
}

SwapchainKHR::~SwapchainKHR() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice.destroySwapchainKHR(m_swapchainKHR);
}
