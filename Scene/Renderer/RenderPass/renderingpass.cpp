#include "renderingpass.hpp"

RenderingPass::RenderingPass(const Device &device) :
    RenderPass(device)
{
    vk::AttachmentDescription diffuseAttachment(vk::AttachmentDescriptionFlags(),
                                                vk::Format::eR8G8B8A8Unorm,
                                                vk::SampleCountFlagBits::e1,
                                                vk::AttachmentLoadOp::eClear,
                                                vk::AttachmentStoreOp::eDontCare,
                                                vk::AttachmentLoadOp::eDontCare,
                                                vk::AttachmentStoreOp::eDontCare,
                                                vk::ImageLayout::eUndefined,
                                                vk::ImageLayout::eUndefined);

    vk::AttachmentDescription depthAttachment(diffuseAttachment);

    depthAttachment.format = vk::Format::eD24UnormS8Uint;

    // Diffuse Attachment will be used into the presentation engine by shaderRead
    diffuseAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    diffuseAttachment.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    vk::AttachmentDescription description[] = {diffuseAttachment, depthAttachment};

    vk::AttachmentReference diffuseRef(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription staticGeometry(vk::SubpassDescriptionFlags(),
                                          vk::PipelineBindPoint::eGraphics,
                                          0, nullptr,
                                          1, &diffuseRef,
                                          nullptr, &depthRef, 0, nullptr);

    vk::RenderPassCreateInfo info(vk::RenderPassCreateFlags(),
                                  2, description,
                                  1, &staticGeometry,
                                  0, nullptr);

    m_renderPass = device.createRenderPass(info);
}
