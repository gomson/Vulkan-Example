#pragma once
#include "VkTools/Pipeline/pipeline.hpp"
#include "VkTools/Pipeline/renderpass.hpp"

class PipelineToPresent : public Pipeline
{
public:
    PipelineToPresent(Device const &device, RenderPass renderPass);
};
