#include "geometryrenderpass.hpp"

GeometryRenderPass::GeometryRenderPass(const Device &device) :
    RenderPass(device)
{
    vk::AttachmentDescription diffuseAttachment(vk::AttachmentDescriptionFlags(),
                                                vk::Format::eR8G8B8A8Unorm,
                                                vk::SampleCountFlagBits::e1,
                                                vk::AttachmentLoadOp::eClear,
                                                vk::AttachmentStoreOp::eStore,
                                                vk::AttachmentLoadOp::eDontCare,
                                                vk::AttachmentStoreOp::eDontCare,
                                                vk::ImageLayout::eUndefined,
                                                vk::ImageLayout::eShaderReadOnlyOptimal);

    vk::AttachmentDescription normalAttachment(diffuseAttachment);
    vk::AttachmentDescription tangeantAttachment(diffuseAttachment);
    vk::AttachmentDescription depthAttachment(diffuseAttachment);

    normalAttachment.format = vk::Format::eR16G16B16A16Sfloat;
    tangeantAttachment.format = vk::Format::eR16G16B16A16Sfloat;
    depthAttachment.format = vk::Format::eD24UnormS8Uint;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;

    vk::AttachmentDescription description[] = {diffuseAttachment, normalAttachment, tangeantAttachment, depthAttachment};

    vk::AttachmentReference diffuseRef(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference normalRef(1, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference tangeantRef(2, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthRef(3, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentReference colorRef[] = {diffuseRef, normalRef, tangeantRef};

    vk::SubpassDescription staticGeometry(vk::SubpassDescriptionFlags(),
                                          vk::PipelineBindPoint::eGraphics,
                                          0, nullptr,
                                          3, colorRef,
                                          nullptr, &depthRef, 0, nullptr);

    vk::RenderPassCreateInfo info(vk::RenderPassCreateFlags(),
                                  4, description,
                                  1, &staticGeometry,
                                  0, nullptr);

    m_renderPass = device.createRenderPass(info);
}
