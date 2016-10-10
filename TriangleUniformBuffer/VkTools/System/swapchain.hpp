#pragma once
#include "device.hpp"
#include "imageview.hpp"
#include "framebuffer.hpp"
#include "../Pipeline/abstractrenderpass.hpp"
#include "vulkan/vkresource.hpp"

class SwapchainKHR : public VkResource, public vk::SwapchainKHR
{
public:
    SwapchainKHR(Device &device, vk::SurfaceKHR surface, AbstractRenderPass &renderpass);

    friend void swap(SwapchainKHR &s1, SwapchainKHR &s2);
    SwapchainKHR(SwapchainKHR &&swapchainKHR);
    SwapchainKHR(SwapchainKHR const &swapchainKHR);
    SwapchainKHR &operator=(SwapchainKHR swapchainKHR);

    FrameBuffer const &getFrameBuffers(uint32_t index) const;
    unsigned getWidth() const;
    unsigned getHeight() const;
    unsigned getImageCount() const;

    void createSwapchainKHR();

    ~SwapchainKHR();

private:
    std::vector<vk::Image> mImages;
    std::vector<FrameBuffer> mFrameBuffer;
    std::vector<ImageView> mImageView;
    unsigned mImageCount;

    vk::PhysicalDevice mPhysicalDevice;
    vk::SurfaceKHR mSurfaceKHR;
    vk::RenderPass mRenderPass;
    vk::PresentModeKHR mMode;
    vk::SurfaceFormatKHR mFormat;
    unsigned mWidth;
    unsigned mHeight;

    void createImageViews();
    void createFrameBuffers();
    vk::SwapchainCreateInfoKHR buildCreateInfos();
};
