#pragma once
#include "VkTools/Pipeline/pipelinelayout.hpp"

class StaticGeometryPipelineLayout : public PipelineLayout
{
public:
    StaticGeometryPipelineLayout(const Device &device);
};

