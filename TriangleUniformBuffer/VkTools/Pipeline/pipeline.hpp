#pragma once
#include "pipelinelayout.hpp"

class Pipeline : public VkResource, public vk::Pipeline
{
protected:
    Pipeline(Device &device,
             PipelineLayout pipelineLayout);

public:
    friend void swap(Pipeline &p1, Pipeline &p2);
    Pipeline(Pipeline &&pipeline);
    Pipeline(Pipeline const &pipeline);
    Pipeline &operator=(Pipeline pipeline);

    PipelineLayout getLayout();
    ~Pipeline();

protected:
    std::shared_ptr<PipelineLayout> mPipelineLayout;
};
