#include <iostream>
#include "swapchain.hpp"

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


vk::Framebuffer const &SwapchainKHR::getFrameBuffers(uint32_t index) const {
    return *mFrameBuffer[index];
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
        mImageView.emplace_back(std::make_shared<ImageView>(mDevice, info));
    }
}

void SwapchainKHR::createFrameBuffers() {
    mFrameBuffer.clear();
    for(auto imageView : mImageView) {
        vk::FramebufferCreateInfo fb(vk::FramebufferCreateFlags(),
                                     mRenderPass, 1, imageView.get(),
                                     mWidth, mHeight, 1);

        mFrameBuffer.emplace_back(std::make_shared<FrameBuffer>(mDevice, fb));
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
    mDevice.destroySwapchainKHR(m_swapchainKHR);
}
