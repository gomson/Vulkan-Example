#include "staticgeometrypipelinelayout.hpp"

StaticGeometryPipelineLayout::StaticGeometryPipelineLayout(const Device &device) :
    PipelineLayout(device) {
    vk::DescriptorSetLayoutBinding uboBinding(0, vk::DescriptorType::eUniformBuffer,
                                              1, vk::ShaderStageFlagBits::eVertex);

    vk::DescriptorSetLayoutCreateInfo layoutInfo(vk::DescriptorSetLayoutCreateFlags(), 1, &uboBinding);

    mDescriptorSetLayouts->emplace_back(device, layoutInfo);

    vk::PipelineLayoutCreateInfo info(vk::PipelineLayoutCreateFlags(),
                                      1, &(*mDescriptorSetLayouts)[0],
                                      0, nullptr);
    m_pipelineLayout = device.createPipelineLayout(info);
}
