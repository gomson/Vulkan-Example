#pragma once
#include "VkTools/Pipeline/pipelinelayout.hpp"
#include "VkTools/Image/sampler.hpp"

class PipelineLayoutToPresent : public PipelineLayout
{
public:
    PipelineLayoutToPresent(Device const &device, std::shared_ptr<Sampler> sampler);

private:
    std::shared_ptr<Sampler> mSampler;
};
