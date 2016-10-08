#pragma once
#include "device.hpp"
#include "imageview.hpp"
#include "framebuffer.hpp"
#include "../Pipeline/abstractrenderpass.hpp"
#include "vulkan/vkresource.hpp"

class SwapchainKHR : public vk::SwapchainKHR, public VkResource
{
public:
    SwapchainKHR(Device &device, vk::SurfaceKHR surface, AbstractRenderPass &renderpass);

    vk::Framebuffer const &getFrameBuffers(uint32_t index) const;
    unsigned getWidth() const;
    unsigned getHeight() const;
    unsigned getImageCount() const;

    void createSwapchainKHR();

    ~SwapchainKHR();

private:
    std::vector<vk::Image> mImages;
    std::vector<std::shared_ptr<FrameBuffer>> mFrameBuffer;
    std::vector<std::shared_ptr<ImageView>> mImageView;
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
