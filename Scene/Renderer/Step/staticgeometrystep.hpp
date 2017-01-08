#pragma once
#include "Renderer/Pipeline/staticgeometrypipeline.hpp"
#include "Renderer/PipelineLayout/staticgeometrypipelinelayout.hpp"
#include "VkTools/Buffer/buffer.hpp"
#include "VkTools/Command/commandpool.hpp"
#include "VkTools/Image/completeframebuffer.hpp"
#include "VkTools/Command/transferer.hpp"
#include "Renderer/Scene/staticnode.hpp"

class StaticGeometryStep
{
public:
    StaticGeometryStep(Device &device, RenderingPass &renderpass, Transferer &transferer, DescriptorSetLayout materialLayout);

    void compileScene(std::vector<CompleteFrameBuffer> framebuffer);

    vk::CommandBuffer getCommandBuffer(uint32_t index) const;

    uint32_t &getRefNumberIndices() {return mNumberIndices;}
    uint32_t &getRefNumberVertices() {return mNumberVertices;}

    Buffer &getRefVbo() {return *mVbo;}
    Buffer &getRefIbo() {return *mIbo;}

    void setRootNode(std::shared_ptr<Node> node) {mRootNode = node;}

private:
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<RenderingPass> mRenderingPass;
    std::shared_ptr<Transferer> mTransferer;
    std::shared_ptr<StaticGeometryPipeline> mPipeline;
    std::shared_ptr<CommandPool> mCommandPool = std::make_shared<CommandPool>(*mDevice, false, false, mDevice->getIndexFamillyQueue());

    std::shared_ptr<Buffer> mVbo = std::make_shared<Buffer>(*mDevice, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc, 1 << 16, mTransferer->getAllocator(), true);
    std::shared_ptr<Buffer> mIbo = std::make_shared<Buffer>(*mDevice, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc, 1 << 16, mTransferer->getAllocator(), true);

    uint32_t mNumberIndices = 0;
    uint32_t mNumberVertices = 0;

    std::shared_ptr<std::vector<vk::CommandBuffer>> mCommandBuffer;

    std::shared_ptr<Node> mRootNode;
};
