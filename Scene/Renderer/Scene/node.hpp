#pragma once
#include "vulkan/vulkan.hpp"

class Node {
public:
    virtual void draw(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout, vk::ShaderStageFlags stageFlags, const glm::mat4 &parent) = 0;
protected:
    std::vector<std::shared_ptr<Node>> mChildren;
    glm::mat4 mWorldMatrix = glm::mat4(1.f);
};
