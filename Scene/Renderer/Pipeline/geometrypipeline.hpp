#pragma once
#include "VkTools/Pipeline/pipeline.hpp"
#include "Renderer/RenderPass/renderingpass.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec2 textureCoordinate;
    glm::vec3 normal;
    glm::vec3 tangeant;
};

class GeometryPipeline : public Pipeline
{
public:
    GeometryPipeline(const Device &device, std::string shaderPrefix,
                     RenderingPass const &renderPass);

};
