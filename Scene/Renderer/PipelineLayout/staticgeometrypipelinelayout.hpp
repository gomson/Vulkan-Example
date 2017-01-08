#pragma once
#include "VkTools/Pipeline/pipelinelayout.hpp"

class StaticGeometryPipelineLayout : public PipelineLayout
{
public:
    StaticGeometryPipelineLayout(Device &device, DescriptorSetLayout layout);
};

