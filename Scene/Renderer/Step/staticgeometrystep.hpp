#pragma once
#include "Renderer/Pipeline/staticgeometrypipeline.hpp"
#include "Renderer/PipelineLayout/staticgeometrypipelinelayout.hpp"
#include "VkTools/Buffer/buffer.hpp"

class StaticGeometryStep
{
public:
    StaticGeometryStep(Device const &device, RenderingPass &renderpass);

private:
    std::shared_ptr<Device> mDevice;
    std::shared_ptr<StaticGeometryPipeline> mPipeline;
    std::shared_ptr<Buffer> mVbo;
    std::shared_ptr<Buffer> mIbo;
};
