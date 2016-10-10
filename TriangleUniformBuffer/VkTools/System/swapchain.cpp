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
    swap(s1.mMode, s2.mMode);
    swap(s1.mPhysicalDevice, s2.mPhysicalDevice);
    swap(s1.mRenderPass, s2.mRenderPass);
    swap(s1.mSurfaceKHR, s2.mSurfaceKHR);
    swap(s1.mWidth, s2.mWidth);
    swap(s1.m_swapchainKHR, s2.m_swapchainKHR);
}

SwapchainKHR::SwapchainKHR(SwapchainKHR &&swapchainKHR) :
    VkResource(std::move(swapchainKHR)),
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
    mMode = swapchainKHR.mMode;
    mPhysicalDevice = swapchainKHR.mPhysicalDevice;
    mRenderPass = swapchainKHR.mRenderPass;
    mSurfaceKHR = swapchainKHR.mSurfaceKHR;
    mWidth = swapchainKHR.mWidth;
}

SwapchainKHR &SwapchainKHR::operator =(SwapchainKHR swapchainKHR) {
    swap(*this, swapchainKHR);
    return *this;
}

SwapchainKHR::SwapchainKHR(Device &device, vk::SurfaceKHR surfaceKHR, AbstractRenderPass &renderPass) :
    VkResource(device),
    mPhysicalDevice(device.getPhysicalDevice()),
    mSurfaceKHR(surfaceKHR),
    mRenderPass(renderPass),
    mMode(vk::PresentModeKHR::eFifo) {
    std::vector<vk::SurfaceFormatKHR> formats(mPhysicalDevice.getSurfaceFormatsKHR(surfaceKHR));

    for(auto &fmt : formats) {
        if(fmt.format == vk::Format::eB8G8R8A8Unorm ||
           fmt.format == vk::Format::eUndefined) {
            mFormat = vk::SurfaceFormatKHR(vk::Format::eB8G8R8A8Unorm);
            break;
        }
    }

    std::vector<vk::PresentModeKHR> modes(mPhysicalDevice.getSurfacePresentModesKHR(surfaceKHR));

    for(auto &m : modes)
        if(m == vk::PresentModeKHR::eMailbox)
            mMode = vk::PresentModeKHR::eMailbox;

    m_swapchainKHR = VK_NULL_HANDLE;
    createSwapchainKHR();
}


FrameBuffer const &SwapchainKHR::getFrameBuffers(uint32_t index) const {
    return mFrameBuffer[index];
}

unsigned SwapchainKHR::getImageCount() const {
    return mImageCount;
}

unsigned SwapchainKHR::getWidth() const {
    return mWidth;
}

unsigned SwapchainKHR::getHeight() const {
    return mHeight;
}

void SwapchainKHR::createImageViews() {
    mImageView.clear();

    for(vk::Image image : mImages) {
        vk::ImageViewCreateInfo info(vk::ImageViewCreateFlags(),
                                     image, vk::ImageViewType::e2D,
                                     mFormat.format, vk::ComponentMapping(),
                                     vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
        mImageView.emplace_back(mDevice, info);
    }
}

void SwapchainKHR::createFrameBuffers() {
    mFrameBuffer.clear();
    for(auto &imageView : mImageView) {
        vk::FramebufferCreateInfo fb(vk::FramebufferCreateFlags(),
                                     mRenderPass, 1, &imageView,
                                     mWidth, mHeight, 1);

        mFrameBuffer.emplace_back(mDevice, fb);
    }
}

vk::SwapchainCreateInfoKHR SwapchainKHR::buildCreateInfos() {
    vk::SurfaceCapabilitiesKHR capabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurfaceKHR);
    vk::Extent2D extent(capabilities.currentExtent);
    mWidth = extent.width;
    mHeight = extent.height;

    mImageCount = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);

    vk::SwapchainCreateInfoKHR info(vk::SwapchainCreateFlagsKHR(), mSurfaceKHR, mImageCount,
                                    mFormat.format, mFormat.colorSpace, extent, 1,
                                    vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive,
                                    0, nullptr, capabilities.currentTransform,
                                    vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                    mMode, true, m_swapchainKHR);

    return info;
}

void SwapchainKHR::createSwapchainKHR() {
    auto result = mDevice.createSwapchainKHR(buildCreateInfos());
    if(m_swapchainKHR != VK_NULL_HANDLE)
        mDevice.destroySwapchainKHR(m_swapchainKHR);
    m_swapchainKHR = result;
    mImages = mDevice.getSwapchainImagesKHR(*this);
    createImageViews();
    createFrameBuffers();
}

SwapchainKHR::~SwapchainKHR() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice.destroySwapchainKHR(m_swapchainKHR);
}
