#pragma once
#include "VkTools/Buffer/buffer.hpp"
#include "VkTools/Command/commandpool.hpp"
#include "Renderer/FrameBuffer/renderingframebuffer.hpp"
#include "Renderer/Pipeline/staticgeometrypipeline.hpp"
#include "VkTools/Command/commandbuffersubmitter.hpp"
#include "VkTools/Command/transferer.hpp"

#include "Renderer/Material/materialdescriptorsetpool.hpp"
#include "staticgeometrystep.hpp"

class RenderingStep
{
public:
    RenderingStep(const Device &device, CommandBufferSubmitter commandBufferSubmitter, Transferer transferer);

    void resize(uint32_t width, uint32_t height, std::shared_ptr<AbstractAllocator> allocator, uint32_t numberRoundRobin);
    std::vector<CompleteFrameBuffer> getFramebuffer() const;

    void execute(uint32_t index);

    uint32_t &getRefNumberStaticIndices() {return mStaticGeometryStep->getRefNumberIndices();}
    uint32_t &getRefNumberStaticVertices() {return mStaticGeometryStep->getRefNumberVertices();}

    Buffer &getRefStaticVbo() {return mStaticGeometryStep->getRefVbo();}
    Buffer &getRefStaticIbo() {return mStaticGeometryStep->getRefIbo();}

    MaterialDescriptorSetManager &getRefMaterialDescriptorSetManager(){return *mMaterialDescriptorSetManager;}

    void setRootNode(std::shared_ptr<Node> node) {mStaticGeometryStep->setRootNode(node);}

private:
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<RenderingPass> mRenderPass = std::make_shared<RenderingPass>(*mDevice);
    std::shared_ptr<std::vector<CompleteFrameBuffer>> mFrameBuffers = std::make_shared<std::vector<CompleteFrameBuffer>>();
    std::shared_ptr<CommandBufferSubmitter> mCommandBufferSubmitter;
    std::shared_ptr<Transferer> mTransferer;

    std::shared_ptr<MaterialDescriptorSetManager> mMaterialDescriptorSetManager = std::make_shared<MaterialDescriptorSetManager>(*mDevice);

    std::shared_ptr<StaticGeometryStep> mStaticGeometryStep = std::make_shared<StaticGeometryStep>(*mDevice, *mRenderPass, *mTransferer, mMaterialDescriptorSetManager->getDescriptorSetLayout());
};
