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

#include "VkTools/Pipeline/abstractpipeline.hpp"
#include "VkTools/Pipeline/abstractrenderpass.hpp"
#include "VkTools/Pipeline/abstractpipelinelayout.hpp"
#include "VkTools/Pipeline/abstractrenderpass.hpp"

#include "VkTools/Memory/bufferimagetransferer.hpp"
#include "VkTools/Memory/deviceallocator.hpp"

struct UniformColor {
    glm::vec4 color;
};

class PipelineLayoutTriangle : public AbstractPipelineLayout {
public:
    PipelineLayoutTriangle(Device &device, DescriptorPool descriptorPool,
                           Buffer buffer) :
        AbstractPipelineLayout(device) {
        // One binding : an uniform buffer
        vk::DescriptorSetLayoutBinding uboLayoutBinding(0,
                                                        vk::DescriptorType::eUniformBuffer,
                                                        1,
                                                        vk::ShaderStageFlagBits::eFragment);

        vk::DescriptorSetLayoutCreateInfo setLayout(
                    vk::DescriptorSetLayoutCreateFlags(),
                    1, &uboLayoutBinding);

        mDescriptorSetLayouts.emplace_back(device.createDescriptorSetLayout(setLayout));

        vk::PipelineLayoutCreateInfo ci(vk::PipelineLayoutCreateFlags(),
                                        mDescriptorSetLayouts.size(),
                                        mDescriptorSetLayouts.data(), 0, nullptr);

        m_pipelineLayout = device.createPipelineLayout(ci);

        vk::DescriptorSetAllocateInfo allocateInfo(descriptorPool,
                                                   mDescriptorSetLayouts.size(),
                                                   mDescriptorSetLayouts.data());

        mDescriptorSet = device.allocateDescriptorSets(allocateInfo);

        vk::DescriptorBufferInfo bufferInfo(buffer, 0, buffer.getSize());
        vk::WriteDescriptorSet descriptorWrite(
                    mDescriptorSet[0], 0, 0, 1,
                    vk::DescriptorType::eUniformBuffer,
                    nullptr, &bufferInfo, nullptr);

        device.updateDescriptorSets({descriptorWrite}, {});

    }
private:
};

// Represents the pipeline for draw a triangle
class PipelineTriangle : public AbstractPipeline {
public:
    PipelineTriangle(Device &device, AbstractRenderPass &renderpass,
                     std::shared_ptr<AbstractPipelineLayout> pipelineLayout) :
        AbstractPipeline(device, pipelineLayout) {
        mShaders.emplace_back(std::make_unique<ShaderModule>(device, "../Shaders/triangle_vert.spv"));
        mShaders.emplace_back(std::make_unique<ShaderModule>(device, "../Shaders/triangle_frag.spv"));

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

        // VertexShader with only one entry
        vk::VertexInputBindingDescription bindingDescription(0, sizeof(glm::vec2), vk::VertexInputRate::eVertex);
        vk::VertexInputAttributeDescription attributeDescription(0, 0, vk::Format::eR32G32Sfloat);

        // Vertices are specified into the shaders
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo(vk::PipelineVertexInputStateCreateFlags(),
                                                               1, &bindingDescription,
                                                               1, &attributeDescription);

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
class RenderPassTriangle : public AbstractRenderPass {
public:
    RenderPassTriangle(Device &device) :
        AbstractRenderPass(device) {
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
                            AbstractPipeline &pipeline,
                            SwapchainKHR swapchainKHR,
                            AbstractRenderPass &renderPass,
                            uint32_t imageIndex,
                            Buffer buffer) {
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
                                  *pipeline.getLayout(),
                                  0, pipeline.getLayout()->getDescriptorSet(), {});
    drawBuffer.setScissor(0, {scissor});
    drawBuffer.setViewport(0, {viewPort});
    drawBuffer.bindVertexBuffers(0, {buffer}, {0});
    drawBuffer.draw(3, 1, 0, 0); // One triangle
    drawBuffer.end();
}

// The primary command buffer which is recorded at each frame
void buildPrimaryCommandBuffers(AbstractRenderPass &renderPass,
                                vk::CommandBuffer &primaryBuffer,
                                SwapchainKHR swapchainKHR,
                                vk::CommandBuffer &drawBuffer,
                                uint32_t imageIndex) {
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

void createBuffers(Device &device,
                   Buffer &vertexBuffer,
                   Buffer &uniformBuffer,
                   BufferImageTransferer &bufferImageTransfer,
                   std::shared_ptr<AbstractAllocator> allocator) {
    std::vector<glm::vec2> triangle({glm::vec2(0.0, -0.5), glm::vec2(0.5, 0.5), glm::vec2(-0.5, 0.5)});
    glm::vec4 color = glm::vec4(1.0, 0.0, 1.0, 1.0);
    vk::BufferUsageFlags bufferUsage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eUniformBuffer;

    vk::BufferUsageFlags stagingUsage = bufferUsage | vk::BufferUsageFlagBits::eTransferSrc;
    vk::BufferUsageFlags gpuUsage = bufferUsage | vk::BufferUsageFlagBits::eTransferDst;

    vertexBuffer = Buffer(device, gpuUsage, triangle.size() * sizeof(glm::vec2), allocator);
    Buffer cpuVertexBuffer(device, stagingUsage, vk::ArrayProxy<glm::vec2>(triangle), allocator);

    uniformBuffer = Buffer(device, gpuUsage, sizeof color, allocator);
    Buffer cpuUniformBuffer(device, stagingUsage, sizeof color, &color, allocator);

    bufferImageTransfer.transfer(cpuVertexBuffer, vertexBuffer, 0, 0, vertexBuffer.getSize()); // We transfer the stagingBuffer to gpuBuffer
    bufferImageTransfer.transfer(cpuUniformBuffer, uniformBuffer, 0, 0, uniformBuffer.getSize());
    bufferImageTransfer.flush(); // We wait for all transfer be finished
}

auto createDescriptorPool(Device &device) {
    vk::DescriptorPoolSize size(vk::DescriptorType::eUniformBuffer, 1);

    return DescriptorPool(device, 1, vk::ArrayProxy<vk::DescriptorPoolSize>(size));
}

int main(int argc, char *argv[])
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

    Buffer gpuBuffer;
    Buffer uniformBuffer;

    createBuffers(device, gpuBuffer, uniformBuffer,
                  bufferImageTransfer, deviceAllocator);

    DescriptorPool descriptorPool = createDescriptorPool(device);

    std::shared_ptr<PipelineLayoutTriangle> pipelineLayout = std::make_shared<PipelineLayoutTriangle>(device, descriptorPool, uniformBuffer);
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
    Fence fences[] = {Fence(device, true), Fence(device, true), Fence(device, true)};

    while(!window.isClosed()) {
        glfwPollEvents();

        /* If window is resized, we re create the swapchain and we reset all buffers
            Then, we build again drawBuffer */
        if(window.isResized()) {
            device.waitIdle();
            swapchainKHR.createSwapchainKHR();

            drawBufferPool.reset(false);

            for(auto i = 0u; i < swapchainKHR.getImageCount(); ++i)
                buildDrawCommandBuffer(drawBuffers[i], pipeline, swapchainKHR, renderPass, i, gpuBuffer);
        }

        // We get the next index and ask for signaling the imageAvailableSemaphore
        auto index = device.acquireNextImageKHR(swapchainKHR, UINT64_MAX, imageAvailableSemaphore, vk::Fence()).value;

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
