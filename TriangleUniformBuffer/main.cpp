#include <iostream>
#include <memory>

#include "VkTools/System/window.hpp"
#include "VkTools/System/instance.hpp"
#include "VkTools/System/device.hpp"
#include "VkTools/System/swapchain.hpp"
#include "VkTools/System/shadermodule.hpp"
#include "VkTools/System/commandpool.hpp"
#include "VkTools/System/semaphore.hpp"
#include "VkTools/System/fence.hpp"
#include "VkTools/System/descriptorpool.hpp"

#include "VkTools/Pipeline/pipeline.hpp"
#include "VkTools/Pipeline/renderpass.hpp"
#include "VkTools/Pipeline/pipelinelayout.hpp"
#include "VkTools/Pipeline/renderpass.hpp"

#include "VkTools/Memory/bufferimagetransferer.hpp"
#include "VkTools/Memory/deviceallocator.hpp"

struct UniformColor {
    glm::vec4 color;
};

class PipelineLayoutTriangle : public PipelineLayout {
public:
    PipelineLayoutTriangle(Device &device, DescriptorPool descriptorPool,
                           Buffer buffer) :
        PipelineLayout(device) {
        // One binding : an uniform buffer
        vk::DescriptorSetLayoutBinding uboLayoutBinding(0,
                                                        vk::DescriptorType::eUniformBuffer,
                                                        1,
                                                        vk::ShaderStageFlagBits::eFragment);

        vk::DescriptorSetLayoutCreateInfo setLayout(
                    vk::DescriptorSetLayoutCreateFlags(),
                    1, &uboLayoutBinding);

        mDescriptorSetLayouts->emplace_back(device.createDescriptorSetLayout(setLayout));

        vk::PipelineLayoutCreateInfo ci(vk::PipelineLayoutCreateFlags(),
                                        mDescriptorSetLayouts->size(),
                                        mDescriptorSetLayouts->data(), 0, nullptr);

        m_pipelineLayout = device.createPipelineLayout(ci);

        vk::DescriptorSetAllocateInfo allocateInfo(descriptorPool,
                                                   mDescriptorSetLayouts->size(),
                                                   mDescriptorSetLayouts->data());

        *mDescriptorSets = device.allocateDescriptorSets(allocateInfo);

        vk::DescriptorBufferInfo bufferInfo(buffer, 0, buffer.getSize());
        vk::WriteDescriptorSet descriptorWrite(
                    (*mDescriptorSets)[0], 0, 0, 1,
                    vk::DescriptorType::eUniformBuffer,
                    nullptr, &bufferInfo, nullptr);

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
        mShaders.emplace_back(std::make_unique<ShaderModule>(device, "../Shaders/shader_vert.spv"));
        mShaders.emplace_back(std::make_unique<ShaderModule>(device, "../Shaders/shader_frag.spv"));

        std::vector<vk::PipelineShaderStageCreateInfo> stageShaderCreateInfo;

        // Shader to draw a triangle
        stageShaderCreateInfo.emplace_back(vk::PipelineShaderStageCreateFlags(),
                                           vk::ShaderStageFlagBits::eVertex,
                                           *mShaders[0], "main",
                                           nullptr);

        stageShaderCreateInfo.emplace_back(vk::PipelineShaderStageCreateFlags(),
                                           vk::ShaderStageFlagBits::eFragment,
                                           *mShaders[1], "main",
                                           nullptr);

        // Vertices are specified into the shaders
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo(vk::PipelineVertexInputStateCreateFlags(), 0,
                                                               nullptr, 0, nullptr);

        // We want to draw triangles by list
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly(vk::PipelineInputAssemblyStateCreateFlags(),
                                                               vk::PrimitiveTopology::eTriangleList);

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
    std::vector<std::unique_ptr<ShaderModule>> mShaders;
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

        // Two dependencies one for before the subpass and one for after
        std::vector<vk::SubpassDependency> dependencies;

        dependencies.emplace_back(VK_SUBPASS_EXTERNAL, 0,
                                  vk::PipelineStageFlagBits::eBottomOfPipe,
                                  vk::PipelineStageFlagBits::eColorAttachmentOutput, // we want to write at this stage
                                  vk::AccessFlagBits::eMemoryRead,
                                  vk::AccessFlagBits::eColorAttachmentWrite, // We only want to write
                                  vk::DependencyFlagBits::eByRegion);

        dependencies.emplace_back(0, VK_SUBPASS_EXTERNAL,
                                  vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                  vk::PipelineStageFlagBits::eBottomOfPipe, // Need to wait this stage before the presentation
                                  vk::AccessFlagBits::eColorAttachmentWrite,
                                  vk::AccessFlagBits::eMemoryRead, // Presentation only need to read
                                  vk::DependencyFlagBits::eByRegion);

        // This subpass is a graphic one
        vk::SubpassDescription subPass(vk::SubpassDescriptionFlags(),
                                       vk::PipelineBindPoint::eGraphics, 0, nullptr,
                                       1, &colorAttachmentRef);

        vk::RenderPassCreateInfo renderpass(vk::RenderPassCreateFlags(),
                                            1, &colorAttachment, 1, &subPass, dependencies.size(),
                                            dependencies.data());

        m_renderPass = device.createRenderPass(renderpass);
    }
};

// This function create a secondary buffer that performs draw
void buildDrawCommandBuffer(vk::CommandBuffer &drawBuffer,
                            Pipeline &pipeline,
                            SwapchainKHR swapchainKHR,
                            RenderPass &renderPass,
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
                                  0, pipeline.getLayout().getDescriptorSets(), {});
    drawBuffer.setScissor(0, {scissor});
    drawBuffer.setViewport(0, {viewPort});
    drawBuffer.draw(3, 1, 0, 0); // One triangle
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

Buffer createBuffers(Device &device,
                   BufferImageTransferer bufferImageTransfer,
                   std::shared_ptr<AbstractAllocator> allocator) {
    glm::vec4 color = glm::vec4(1.0, 0.0, 1.0, 1.0);
    vk::BufferUsageFlags bufferUsage = vk::BufferUsageFlagBits::eUniformBuffer;

    vk::BufferUsageFlags stagingUsage = bufferUsage | vk::BufferUsageFlagBits::eTransferSrc;
    vk::BufferUsageFlags gpuUsage = bufferUsage | vk::BufferUsageFlagBits::eTransferDst;

    Buffer uniformBuffer(device, gpuUsage, sizeof color, allocator);
    Buffer cpuUniformBuffer(device, stagingUsage, sizeof color, &color, allocator);

    bufferImageTransfer.transfer(cpuUniformBuffer, uniformBuffer, 0, 0, uniformBuffer.getSize());
    bufferImageTransfer.flush(); // We wait for all transfer be finished

    return uniformBuffer;
}

auto createDescriptorPool(Device &device) {
    vk::DescriptorPoolSize size(vk::DescriptorType::eUniformBuffer, 1);

    return DescriptorPool(device, 1, vk::ArrayProxy<vk::DescriptorPoolSize>(size));
}

int main()
{
    Window window(1024, 768);
    Instance instance(window, true);
    Device device(instance);

    vk::Queue queue(device.getGraphicQueue());
    // Custom allocator on heap device (host visible or device local)
    std::shared_ptr<DeviceAllocator> deviceAllocator(std::make_shared<DeviceAllocator>(device, 1 << 24));
    BufferImageTransferer bufferImageTransfer(device, 100);

    RenderPassTriangle renderPass(device);
    SwapchainKHR swapchainKHR(device, instance.getSurfaceKHR(), renderPass);

    Buffer uniformBuffer = createBuffers(device, bufferImageTransfer,
                                         deviceAllocator);

    DescriptorPool descriptorPool = createDescriptorPool(device);

    PipelineLayoutTriangle pipelineLayout = PipelineLayoutTriangle(device, descriptorPool, uniformBuffer);
    PipelineTriangle pipeline(device, renderPass, pipelineLayout);

    /* 2 pools,
     * 	one for primaryBuffer which is a transient one : short lived commandBuffer
     * 	one for drawCall, which is recorded when we resize the window */
    CommandPool primaryBufferPool(device, true, true, device.getIndexFamillyQueue());
    CommandPool drawBufferPool(device, false, false, device.getIndexFamillyQueue());

    std::vector<vk::CommandBuffer> primaryCommandBuffers = primaryBufferPool.allocate(vk::CommandBufferLevel::ePrimary, swapchainKHR.getImageCount());
    std::vector<vk::CommandBuffer> drawBuffers = drawBufferPool.allocate(vk::CommandBufferLevel::eSecondary, swapchainKHR.getImageCount());

    Semaphore imageAvailableSemaphore(device);
    Semaphore imageRenderFinishedSemaphore(device);
    std::vector<Fence> fences;

    for(int i = 0; i < swapchainKHR.getImageCount(); ++i)
        fences.push_back(Fence(device, true));

    while(!window.isClosed()) {
        glfwPollEvents();

        /* If window is resized, we re create the swapchain and we reset all buffers
                Then, we build again drawBuffer */
        if(window.isResized()) {
            device.waitIdle();

            if(window.isSurfaceKHROutOfDate())
                instance.createSurfaceKHR();

            swapchainKHR = SwapchainKHR(device, instance.getSurfaceKHR(), renderPass, swapchainKHR);

            drawBufferPool.reset(false);

            for(auto i = 0u; i < swapchainKHR.getImageCount(); ++i)
                buildDrawCommandBuffer(drawBuffers[i], pipeline, swapchainKHR, renderPass, i);
            auto a = std::move(swapchainKHR);
            swapchainKHR = std::move(a);
        }

        // We get the next index and ask for signaling the imageAvailableSemaphore
        uint32_t index;
        vk::Result result = device.acquireNextImageKHR(swapchainKHR, UINT64_MAX, imageAvailableSemaphore, vk::Fence(), &index);

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
