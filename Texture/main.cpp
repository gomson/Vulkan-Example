#include <iostream>
#include <memory>

#include "VkTools/System/window.hpp"
#include "VkTools/System/instance.hpp"
#include "VkTools/System/device.hpp"
#include "VkTools/System/swapchain.hpp"
#include "VkTools/System/shadermodule.hpp"
#include "VkTools/Command/commandpool.hpp"
#include "VkTools/Synchronization/semaphore.hpp"
#include "VkTools/Synchronization/fence.hpp"
#include "VkTools/Descriptor/descriptorpool.hpp"

#include "VkTools/Pipeline/pipeline.hpp"
#include "VkTools/Pipeline/renderpass.hpp"
#include "VkTools/Pipeline/pipelinelayout.hpp"
#include "VkTools/Pipeline/renderpass.hpp"

#include "VkTools/Command/transferer.hpp"
#include "VkTools/Memory/deviceallocator.hpp"

#include "VkTools/Image/image.hpp"
#include "VkTools/Image/imageloader.hpp"
#include "VkTools/Image/sampler.hpp"

class PipelineLayoutTriangle : public PipelineLayout {
public:
    PipelineLayoutTriangle(Device &device, DescriptorPool &descriptorPool, vk::DescriptorSet &descriptorSet,
                           ImageView image, Sampler sampler) :
        PipelineLayout(device) {
        // One binding : an uniform buffer
        vk::DescriptorSetLayoutBinding textureLayoutBinding(0,
                                                            vk::DescriptorType::eCombinedImageSampler,
                                                            1,
                                                            vk::ShaderStageFlagBits::eFragment);

        vk::DescriptorSetLayoutCreateInfo setLayout(
                    vk::DescriptorSetLayoutCreateFlags(),
                    1, &textureLayoutBinding);

        mDescriptorSetLayouts->emplace_back(device.createDescriptorSetLayout(setLayout));

        vk::PipelineLayoutCreateInfo ci(vk::PipelineLayoutCreateFlags(),
                                        mDescriptorSetLayouts->size(),
                                        mDescriptorSetLayouts->data(), 0, nullptr);

        m_pipelineLayout = device.createPipelineLayout(ci);

        descriptorPool = DescriptorPool(device, 1, {vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1)}, mDescriptorSetLayouts->front());

        descriptorSet = descriptorPool.allocate();

        vk::DescriptorImageInfo imageInfo(sampler, image, vk::ImageLayout::eShaderReadOnlyOptimal);
        vk::WriteDescriptorSet descriptorWrite(
                    descriptorSet, 0, 0, 1,
                    vk::DescriptorType::eCombinedImageSampler,
                    &imageInfo, nullptr, nullptr);

        device.updateDescriptorSets({descriptorWrite}, {});

    }
private:
};

// Represents the pipeline for draw a triangle
class PipelineTriangle : public Pipeline {
public:
    PipelineTriangle(Device &device, RenderPass &renderpass,
                     PipelineLayout pipelineLayout) :
        Pipeline(device, pipelineLayout) {
        mShaderModules->emplace_back(device, "../Shaders/shader_vert.spv");
        mShaderModules->emplace_back(device, "../Shaders/shader_frag.spv");

        std::vector<vk::PipelineShaderStageCreateInfo> stageShaderCreateInfo;

        // Shader to draw a triangle
        stageShaderCreateInfo.emplace_back(vk::PipelineShaderStageCreateFlags(),
                                           vk::ShaderStageFlagBits::eVertex,
                                           (*mShaderModules)[0], "main",
                                           nullptr);

        stageShaderCreateInfo.emplace_back(vk::PipelineShaderStageCreateFlags(),
                                           vk::ShaderStageFlagBits::eFragment,
                                           (*mShaderModules)[1], "main",
                                           nullptr);

        // Vertices are specified into the shaders
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo(vk::PipelineVertexInputStateCreateFlags(), 0,
                                                               nullptr, 0, nullptr);

        // We want to draw triangles by list
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly(vk::PipelineInputAssemblyStateCreateFlags(),
                                                               vk::PrimitiveTopology::eTriangleStrip);

        // No culling, we do not want a draw by line
        vk::PipelineRasterizationStateCreateInfo rasterizer(vk::PipelineRasterizationStateCreateFlags(), false,
                                                            false, vk::PolygonMode::eFill,
                                                            vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise, false,
                                                            0.f, 0.f, 0.f, 1.f);

        vk::PipelineMultisampleStateCreateInfo ms(vk::PipelineMultisampleStateCreateFlags(),
                                                  vk::SampleCountFlagBits::e1, false,
                                                  1.f, nullptr, false, false);

        // We do not want to use blending
        vk::PipelineColorBlendAttachmentState cba(false);
        cba.colorWriteMask = vk::ColorComponentFlagBits::eA |
                             vk::ColorComponentFlagBits::eB |
                             vk::ColorComponentFlagBits::eG |
                             vk::ColorComponentFlagBits::eR;

        vk::PipelineColorBlendStateCreateInfo cb(vk::PipelineColorBlendStateCreateFlags(), false);
        cb.setAttachmentCount(1).setPAttachments(&cba);

        // We want to use dynamic viewport if we want to resize the window
        std::array<vk::DynamicState, 2> dynamicStateVp{vk::DynamicState::eViewport, vk::DynamicState::eScissor};

        vk::PipelineDynamicStateCreateInfo dynamicState(vk::PipelineDynamicStateCreateFlags(),
                                                        2, dynamicStateVp.data());

        vk::PipelineViewportStateCreateInfo viewportInfo(vk::PipelineViewportStateCreateFlags(),
                                                         1, nullptr, 1, nullptr);


        vk::GraphicsPipelineCreateInfo ci(vk::PipelineCreateFlags(),
                                          2, stageShaderCreateInfo.data(),
                                          &vertexInputInfo,
                                          &inputAssembly, nullptr, &viewportInfo, &rasterizer,
                                          &ms, nullptr, &cb, &dynamicState, *mPipelineLayout,
                                          renderpass, 0);
        m_pipeline = device.createGraphicsPipeline(vk::PipelineCache(), ci);
    }


private:
};

// A renderPass for our triangle
class RenderPassTriangle : public RenderPass {
public:
    RenderPassTriangle(Device &device) :
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

        vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

        // This subpass is a graphic one
        vk::SubpassDescription subPass(vk::SubpassDescriptionFlags(),
                                       vk::PipelineBindPoint::eGraphics, 0, nullptr,
                                       1, &colorAttachmentRef);

        vk::RenderPassCreateInfo renderpass(vk::RenderPassCreateFlags(),
                                            1, &colorAttachment, 1, &subPass, 0, nullptr);

        m_renderPass = device.createRenderPass(renderpass);
    }
};

// This function create a secondary buffer that performs draw
void buildDrawCommandBuffer(vk::CommandBuffer &drawBuffer,
                            Pipeline &pipeline,
                            SwapchainKHR swapchainKHR,
                            RenderPass &renderPass,
                            vk::DescriptorSet descriptorSet,
                            uint32_t imageIndex) {
    // subpass 0 at that frameBuffer
    vk::CommandBufferInheritanceInfo inheritanceInfo(renderPass,
                                                     0,
                                                     swapchainKHR.getFrameBuffers(imageIndex));
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritanceInfo);

    drawBuffer.begin(beginInfo);
    vk::Viewport viewPort(0, 0, swapchainKHR.getWidth(), swapchainKHR.getHeight(), 0.f, 1.f);
    vk::Rect2D scissor(vk::Offset2D(0, 0), vk::Extent2D(swapchainKHR.getWidth(), swapchainKHR.getHeight()));

    drawBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    drawBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                  pipeline.getLayout(),
                                  0, descriptorSet, {});
    drawBuffer.setScissor(0, {scissor});
    drawBuffer.setViewport(0, {viewPort});
    drawBuffer.draw(4, 1, 0, 0); // One triangle
    drawBuffer.end();
}

// The primary command buffer which is recorded at each frame
void buildPrimaryCommandBuffers(RenderPass &renderPass,
                                vk::CommandBuffer &primaryBuffer,
                                SwapchainKHR &swapchainKHR,
                                vk::CommandBuffer &drawBuffer,
                                uint32_t &imageIndex) {
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr);
    primaryBuffer.begin(beginInfo);

    vk::ClearValue clear;
    clear.color = vk::ClearColorValue(std::array<float, 4>{{0, 0, 0, 1}});
    vk::RenderPassBeginInfo renderPassBeginInfo(renderPass,
                                                swapchainKHR.getFrameBuffers(imageIndex),
                                                vk::Rect2D({0, 0},
                                                           {swapchainKHR.getWidth(), swapchainKHR.getHeight()}),
                                                1, &clear);

    // This pass will only use secondary command buffer
    primaryBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eSecondaryCommandBuffers);

    primaryBuffer.executeCommands({drawBuffer});

    primaryBuffer.endRenderPass();
    primaryBuffer.end();
}

int main()
{
    Window window(1024, 768);
    Instance instance(window, true);
    Device device(instance);

    vk::Queue queue(device.getGraphicQueue());
    // Custom allocator on heap device (host visible or device local)
    CommandPool primaryBufferPool(device, true, true, device.getIndexFamillyQueue());
    std::shared_ptr<DeviceAllocator> deviceAllocator(std::make_shared<DeviceAllocator>(device, 1 << 24));
    CommandBufferSubmitter commandBufferSubmitter(device, primaryBufferPool, 1);
    Transferer imageTransfer(0, 1 << 21, deviceAllocator, commandBufferSubmitter);

    RenderPassTriangle renderPass(device);
    SwapchainKHR swapchainKHR(device, instance.getSurfaceKHR(), renderPass);

    DescriptorPool descriptorPool;

    Sampler sampler(device, 1.0);
    Image image; ImageView imageView;

    Image::createImageFromPath("../texture.jpg", image, imageView, imageTransfer);

    vk::DescriptorSet descriptorSet;

    PipelineLayoutTriangle pipelineLayout = PipelineLayoutTriangle(device, descriptorPool, descriptorSet, imageView, sampler);
    PipelineTriangle pipeline(device, renderPass, pipelineLayout);

    CommandPool drawBufferPool(device, false, false, device.getIndexFamillyQueue());

    std::vector<vk::CommandBuffer> primaryCommandBuffers = primaryBufferPool.allocate(vk::CommandBufferLevel::ePrimary, swapchainKHR.getImageCount());
    std::vector<vk::CommandBuffer> drawBuffers = drawBufferPool.allocate(vk::CommandBufferLevel::eSecondary, swapchainKHR.getImageCount());

    Semaphore imageAvailableSemaphore(device);
    Semaphore imageRenderFinishedSemaphore(device);
    std::vector<Fence> fences;

    commandBufferSubmitter.submit();

    for(int i = 0; i < swapchainKHR.getImageCount(); ++i)
        fences.push_back(Fence(device, true));

    while(!window.isClosed()) {
        glfwPollEvents();

        /* If window is resized, we re create the swapchain and we reset all buffers
            Then, we build again drawBuffer */
        if(window.isResized()) {
            device.waitIdle();

            if(window.isSurfaceKHROutOfDate()) {
                swapchainKHR = SwapchainKHR();
                instance.createSurfaceKHR();
            }

            swapchainKHR = SwapchainKHR(device, instance.getSurfaceKHR(), renderPass, swapchainKHR);

            drawBufferPool.reset(false);

            for(auto i = 0u; i < swapchainKHR.getImageCount(); ++i)
                buildDrawCommandBuffer(drawBuffers[i], pipeline, swapchainKHR, renderPass, descriptorSet, i);
        }

        // We get the next index and ask for signaling the imageAvailableSemaphore
        uint32_t index;
        auto result = device.acquireNextImageKHR(swapchainKHR, UINT64_MAX, imageAvailableSemaphore, vk::Fence(), &index);

        if(result == vk::Result::eErrorOutOfDateKHR) {
            window.surfaceIsOutOfDate();
            continue;
        }

        // We wait and reset the current fence
        fences[index].wait();
        fences[index].reset();

        // We build the primaryCommandBuffer : the reset is implicit
        buildPrimaryCommandBuffers(renderPass,
                                   primaryCommandBuffers[index],
                                   swapchainKHR,
                                   drawBuffers[index],
                                   index);

        /* We ask for wait at the colorAttachmentOutput stage because
         * it is this stage that we write to frameBuffer, then we signal the
           imageRenderFinishedSemaphore */
        vk::PipelineStageFlags sf[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        vk::SubmitInfo si(1, &imageAvailableSemaphore, sf, 1, &primaryCommandBuffers[index],
                          1, &imageRenderFinishedSemaphore);

        queue.submit(si, fences[index]);

        // When the imageRenderFinishedSemaphore is signaled we present the swapchain
        vk::PresentInfoKHR present(1, &imageRenderFinishedSemaphore, 1, &swapchainKHR, &index, nullptr);
        queue.presentKHR(present);
    }

    device.waitIdle(); // We wait for the end of all operations for destroy all objects

    return 0;
}
