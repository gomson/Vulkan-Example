#include "staticgeometrypipelinelayout.hpp"

StaticGeometryPipelineLayout::StaticGeometryPipelineLayout(const Device &device) :
    PipelineLayout(device) {
    vk::PushConstantRange pushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4));
    vk::PipelineLayoutCreateInfo info(vk::PipelineLayoutCreateFlags(), 0, nullptr, 1, &pushConstantRange);
    m_pipelineLayout = device.createPipelineLayout(info);
}
