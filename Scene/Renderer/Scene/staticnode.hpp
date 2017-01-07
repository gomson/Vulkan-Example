#pragma once
#include "Renderer/Model/modelloader.hpp"
#include "node.hpp"

class StaticNode : public Node
{
public:
    StaticNode(ModelLoader_t model);

    void draw(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout,
              vk::ShaderStageFlags stageFlags, glm::mat4 const &parent);

private:
    ModelLoader_t mModel;
};
