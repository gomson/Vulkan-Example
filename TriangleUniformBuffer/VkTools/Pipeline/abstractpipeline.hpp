#pragma once
#include "abstractpipelinelayout.hpp"

class AbstractPipeline : public vk::Pipeline, public VkResource, private NotCopyable
{
protected:
    AbstractPipeline(Device &device,
                     std::shared_ptr<AbstractPipelineLayout> pipelineLayout);

public:
    std::shared_ptr<AbstractPipelineLayout> getLayout();
    ~AbstractPipeline();

protected:
    std::shared_ptr<AbstractPipelineLayout> mPipelineLayout;
};
