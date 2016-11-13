#pragma once
#include "device.hpp"
#include "imageview.hpp"
#include "framebuffer.hpp"
#include "../Pipeline/renderpass.hpp"
#include "vulkan/vkresource.hpp"

class SwapchainKHR : public VkResource, public vk::SwapchainKHR
{
public:
    SwapchainKHR(Device &device, vk::SurfaceKHR surface, RenderPass &renderpass,
                 vk::SwapchainKHR oldSwapchainKHR = VK_NULL_HANDLE);

    SwapchainKHR(SwapchainKHR &&swapchainKHR) = default;
    SwapchainKHR(SwapchainKHR const &swapchainKHR) = default;
    SwapchainKHR &operator=(SwapchainKHR const &swapchainKHR) = default;

    FrameBuffer const &getFrameBuffers(uint32_t index) const;
    unsigned getWidth() const;
    unsigned getHeight() const;
    unsigned getImageCount() const;

    ~SwapchainKHR();

private:
    std::shared_ptr<std::vector<vk::Image>> mImages;
    std::shared_ptr<std::vector<FrameBuffer>> mFrameBuffer;
    std::shared_ptr<std::vector<ImageView>> mImageView;
    std::shared_ptr<uint32_t> mImageCount;

    std::shared_ptr<vk::RenderPass> mRenderPass;
    std::shared_ptr<vk::SurfaceFormatKHR> mFormat;
    std::shared_ptr<uint32_t> mWidth;
    std::shared_ptr<uint32_t> mHeight;

    void createImageViews();
    void createFrameBuffers();
    vk::SwapchainCreateInfoKHR buildCreateInfos(vk::SurfaceKHR surfaceKHR, vk::PresentModeKHR mode,
                                                vk::SwapchainKHR oldSwapchainKHR);
};
