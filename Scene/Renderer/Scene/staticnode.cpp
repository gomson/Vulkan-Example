#include "staticnode.hpp"

StaticNode::StaticNode(ModelLoader_t model) : mModel(model) {

}

void StaticNode::draw(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout, vk::ShaderStageFlags stageFlags, const glm::mat4 &parent) {
    glm::mat4 mat = parent * mWorldMatrix;

    commandBuffer.pushConstants<glm::mat4>(pipelineLayout, stageFlags, 0, mat);

    for(auto const &m: mModel.mesheLoaders)
        commandBuffer.drawIndexed(m.indexCount, 1, m.firstIndex, m.vertexOffset, 0);

    for(auto child : mChildren)
        child->draw(commandBuffer, pipelineLayout, stageFlags, mat);
}
