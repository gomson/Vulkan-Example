#pragma once
#include "vulkan/vulkan.hpp"

class Node {
public:
    virtual void draw(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout, vk::ShaderStageFlags stageFlags, const glm::mat4 &parent) = 0;

    void translate(glm::vec3 translation) {
        mWorldMatrix = glm::translate(mWorldMatrix, translation);
    }

    void rotation(glm::vec3 axe, float angle) {
        mWorldMatrix = glm::rotate(mWorldMatrix, angle, axe);
    }

    void scale(glm::vec3 scale) {
        mWorldMatrix = glm::scale(mWorldMatrix, scale);
    }

    void addChild(std::shared_ptr<Node> node) {
        mChildren.emplace_back(node);
    }

protected:
    std::vector<std::shared_ptr<Node>> mChildren;
    glm::mat4 mWorldMatrix = glm::mat4(1.f);
};
