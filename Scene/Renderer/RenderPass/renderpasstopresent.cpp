#include "renderpasstopresent.hpp"

// A renderPass to render into our swapchain
RenderPassToPresent::RenderPassToPresent(const Device &device) :
    RenderPass(device) {
    /* 1 coolor buffer with B8G8R8A8 way
         * we clear at the beginning and we save at the end
         * no stencil
         * oldLayout is undefined because we clear the buffer
         * newLayout is presentSRC because we want to present the image */
    vk::AttachmentDescription colorAttachment(vk::AttachmentDescriptionFlags(),
                                              vk::Format::eB8G8R8A8Unorm, vk::SampleCountFlagBits::e1,
                                              vk::AttachmentLoadOp::eClear,
                                              vk::AttachmentStoreOp::eStore,
                                              vk::AttachmentLoadOp::eDontCare,
                                              vk::AttachmentStoreOp::eDontCare,
                                              vk::ImageLayout::eUndefined,
                                              vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subPass(vk::SubpassDescriptionFlags(),
                                   vk::PipelineBindPoint::eGraphics,
                                   0, nullptr, 1, &colorReference);
    vk::RenderPassCreateInfo renderpass(vk::RenderPassCreateFlags(),
                                        1, &colorAttachment, 1, &subPass, 0, nullptr);

    m_renderPass = device.createRenderPass(renderpass);
}
