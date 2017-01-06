#pragma once
#include "VkTools/Pipeline/pipeline.hpp"
#include "Renderer/RenderPass/renderingpass.hpp"

class StaticGeometryPipeline : public Pipeline
{
public:
    StaticGeometryPipeline(const Device &device, std::string shaderPrefix,
                           RenderingPass &renderPass);

};
