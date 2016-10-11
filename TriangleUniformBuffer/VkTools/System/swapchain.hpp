#pragma once
#include "device.hpp"
#include "imageview.hpp"
#include "framebuffer.hpp"
#include "../Pipeline/renderpass.hpp"
#include "vulkan/vkresource.hpp"

class SwapchainKHR : public VkResource, public vk::SwapchainKHR
{
public:
    SwapchainKHR() = default;
    SwapchainKHR(Device &device, vk::SurfaceKHR surface, RenderPass &renderpass,
                 SwapchainKHR oldSwapchainKHR = SwapchainKHR());

    friend void swap(SwapchainKHR &s1, SwapchainKHR &s2);
    SwapchainKHR(SwapchainKHR &&swapchainKHR);
    SwapchainKHR(SwapchainKHR const &swapchainKHR);
    SwapchainKHR &operator=(SwapchainKHR swapchainKHR);

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
    vk::SwapchainCreateInfoKHR buildCreateInfos(vk::PhysicalDevice physicalDevice,
                                                vk::SurfaceKHR surfaceKHR, vk::PresentModeKHR mode,
                                                SwapchainKHR const &oldSwapchainKHR);
};
