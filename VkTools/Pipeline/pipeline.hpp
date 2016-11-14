#pragma once
#include "pipelinelayout.hpp"

class Pipeline : public VkResource, public vk::Pipeline
{
protected:
    Pipeline(Device &device,
             PipelineLayout pipelineLayout);

public:
    Pipeline(Pipeline &&pipeline) = default;
    Pipeline(Pipeline const &pipeline) = default;
    Pipeline &operator=(Pipeline pipeline);

    PipelineLayout getLayout();
    ~Pipeline();

protected:
    std::shared_ptr<PipelineLayout> mPipelineLayout;
};
