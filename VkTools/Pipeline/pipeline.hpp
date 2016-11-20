#pragma once
#include "pipelinelayout.hpp"
#include "../System/shadermodule.hpp"

class Pipeline : public VkResource, public vk::Pipeline
{
protected:
    Pipeline(Device const &device,
             PipelineLayout const &pipelineLayout);

public:
    Pipeline(Pipeline &&pipeline) = default;
    Pipeline(Pipeline const &pipeline) = default;
    Pipeline &operator=(Pipeline pipeline);

    PipelineLayout getLayout();
    ~Pipeline();

protected:
    std::shared_ptr<PipelineLayout> mPipelineLayout;
    std::shared_ptr<std::vector<ShaderModule>> mShaderModules = std::make_shared<std::vector<ShaderModule>>();
};
