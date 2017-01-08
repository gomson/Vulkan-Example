#include "staticgeometrypipelinelayout.hpp"

StaticGeometryPipelineLayout::StaticGeometryPipelineLayout(Device &device, DescriptorSetLayout materialLayout) :
    PipelineLayout(device) {
    vk::PushConstantRange pushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4));
    vk::PipelineLayoutCreateInfo info(vk::PipelineLayoutCreateFlags(), 1, &materialLayout, 1, &pushConstantRange);
    m_pipelineLayout = device.createPipelineLayout(info);
    mDescriptorSetLayouts->emplace_back(materialLayout);
}
