#pragma once
#include "VkTools/Pipeline/pipeline.hpp"
#include "VkTools/Pipeline/renderpass.hpp"
#include "VkTools/Image/sampler.hpp"

class PipelineToPresent : public Pipeline
{
public:
    PipelineToPresent(Device const &device, RenderPass renderPass,
                      std::shared_ptr<Sampler> sampler);
};
